// Copyright (c) 2021-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <algorithm>
#include <common/args.h>
#include <common/messages.h>
#include <common/system.h>
#include <consensus/amount.h>
#include <consensus/validation.h>
#include <interfaces/chain.h>
#include <node/types.h>
#include <numeric>
#include <policy/policy.h>
#include <primitives/transaction.h>
#include <script/script.h>
#include <script/signingprovider.h>
#include <script/solver.h>
#include <util/check.h>
#include <util/moneystr.h>
#include <util/rbf.h>
#include <util/trace.h>
#include <util/translation.h>
#include <wallet/coincontrol.h>
#include <wallet/fees.h>
#include <wallet/receive.h>
#include <wallet/spend.h>
#include <wallet/transaction.h>
#include <wallet/wallet.h>

#include <cmath>

using common::StringForFeeReason;
using common::TransactionErrorString;
using interfaces::FoundBlock;
using node::TransactionError;

namespace wallet {
static constexpr size_t OUTPUT_GROUP_MAX_ENTRIES{100};

/** Whether the descriptor represents, directly or not, a witness program. */
static bool IsSegwit(const Descriptor& desc) {
    if (const auto typ = desc.GetOutputType()) return *typ != OutputType::LEGACY;
    return false;
}

/** Whether to assume ECDSA signatures' will be high-r. */
static bool UseMaxSig(const std::optional<CTxIn>& txin, const CCoinControl* coin_control) {
    // Use max sig if watch only inputs were used or if this particular input is an external input
    // to ensure a sufficient fee is attained for the requested feerate.
    return coin_control && (coin_control->fAllowWatchOnly || (txin && coin_control->IsExternalSelected(txin->prevout)));
}

/** Get the size of an input (in witness units) once it's signed.
 *
 * @param desc The output script descriptor of the coin spent by this input.
 * @param txin Optionally the txin to estimate the size of. Used to determine the size of ECDSA signatures.
 * @param coin_control Information about the context to determine the size of ECDSA signatures.
 * @param tx_is_segwit Whether the transaction has at least a single input spending a segwit coin.
 * @param can_grind_r Whether the signer will be able to grind the R of the signature.
 */
static std::optional<int64_t> MaxInputWeight(const Descriptor& desc, const std::optional<CTxIn>& txin,
                                             const CCoinControl* coin_control, const bool tx_is_segwit,
                                             const bool can_grind_r) {
    if (const auto sat_weight = desc.MaxSatisfactionWeight(!can_grind_r || UseMaxSig(txin, coin_control))) {
        if (const auto elems_count = desc.MaxSatisfactionElems()) {
            const bool is_segwit = IsSegwit(desc);
            // Account for the size of the scriptsig and the number of elements on the witness stack. Note
            // that if any input in the transaction is spending a witness program, we need to specify the
            // witness stack size for every input regardless of whether it is segwit itself.
            // NOTE: this also works in case of mixed scriptsig-and-witness such as in p2sh-wrapped segwit v0
            // outputs. In this case the size of the scriptsig length will always be one (since the redeemScript
            // is always a push of the witness program in this case, which is smaller than 253 bytes).
            const int64_t scriptsig_len = is_segwit ? 1 : GetSizeOfCompactSize(*sat_weight / WITNESS_SCALE_FACTOR);
            const int64_t witstack_len = is_segwit ? GetSizeOfCompactSize(*elems_count) : (tx_is_segwit ? 1 : 0);
            // previous txid + previous vout + sequence + scriptsig len + witstack size + scriptsig or witness
            // NOTE: sat_weight already accounts for the witness discount accordingly.
            return (32 + 4 + 4 + scriptsig_len) * WITNESS_SCALE_FACTOR + witstack_len + *sat_weight;
        }
    }

    return {};
}

int CalculateMaximumSignedInputSize(const CTxOut& txout, const COutPoint outpoint, const SigningProvider* provider, bool can_grind_r, const CCoinControl* coin_control)
{
    if (!provider) return -1;

    if (const auto desc = InferDescriptor(txout.scriptPubKey, *provider)) {
        if (const auto weight = MaxInputWeight(*desc, {}, coin_control, true, can_grind_r)) {
            return static_cast<int>(GetVirtualTransactionSize(*weight, 0, 0));
        }
    }

    return -1;
}

int CalculateMaximumSignedInputSize(const CTxOut& txout, const CWallet* wallet, const CCoinControl* coin_control)
{
    const std::unique_ptr<SigningProvider> provider = wallet->GetSolvingProvider(txout.scriptPubKey);
    return CalculateMaximumSignedInputSize(txout, COutPoint(), provider.get(), wallet->CanGrindR(), coin_control);
}

/** Infer a descriptor for the given output script. */
static std::unique_ptr<Descriptor> GetDescriptor(const CWallet* wallet, const CCoinControl* coin_control,
                                                 const CScript script_pubkey)
{
    MultiSigningProvider providers;
    for (const auto spkman: wallet->GetScriptPubKeyMans(script_pubkey)) {
        providers.AddProvider(spkman->GetSolvingProvider(script_pubkey));
    }
    if (coin_control) {
        providers.AddProvider(std::make_unique<FlatSigningProvider>(coin_control->m_external_provider));
    }
    return InferDescriptor(script_pubkey, providers);
}

/** Infer the maximum size of this input after it will be signed. */
static std::optional<int64_t> GetSignedTxinWeight(const CWallet* wallet, const CCoinControl* coin_control,
                                                  const CTxIn& txin, const CTxOut& txo, const bool tx_is_segwit,
                                                  const bool can_grind_r)
{
    // If weight was provided, use that.
    std::optional<int64_t> weight;
    if (coin_control && (weight = coin_control->GetInputWeight(txin.prevout))) {
        return weight.value();
    }

    // Otherwise, use the maximum satisfaction size provided by the descriptor.
    std::unique_ptr<Descriptor> desc{GetDescriptor(wallet, coin_control, txo.scriptPubKey)};
    if (desc) return MaxInputWeight(*desc, {txin}, coin_control, tx_is_segwit, can_grind_r);

    return {};
}

// txouts needs to be in the order of tx.vin
TxSize CalculateMaximumSignedTxSize(const CTransaction &tx, const CWallet *wallet, const std::vector<CTxOut>& txouts, const CCoinControl* coin_control)
{
    // version + nLockTime + input count + output count
    int64_t weight = (4 + 4 + GetSizeOfCompactSize(tx.vin.size()) + GetSizeOfCompactSize(tx.vout.size())) * WITNESS_SCALE_FACTOR;
    // Whether any input spends a witness program. Necessary to run before the next loop over the
    // inputs in order to accurately compute the compactSize length for the witness data per input.
    bool is_segwit = std::any_of(txouts.begin(), txouts.end(), [&](const CTxOut& txo) {
        std::unique_ptr<Descriptor> desc{GetDescriptor(wallet, coin_control, txo.scriptPubKey)};
        if (desc) return IsSegwit(*desc);
        return false;
    });
