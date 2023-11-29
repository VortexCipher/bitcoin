// Copyright (c) 2011-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <common/messages.h>
#include <consensus/validation.h>
#include <core_io.h>
#include <key_io.h>
#include <node/types.h>
#include <policy/policy.h>
#include <rpc/rawtransaction_util.h>
#include <rpc/util.h>
#include <script/script.h>
#include <util/rbf.h>
#include <util/translation.h>
#include <util/vector.h>
#include <wallet/coincontrol.h>
#include <wallet/feebumper.h>
#include <wallet/fees.h>
#include <wallet/rpc/util.h>
#include <wallet/spend.h>
#include <wallet/wallet.h>

#include <univalue.h>

using common::FeeModeFromString;
using common::FeeModesDetail;
using common::InvalidEstimateModeErrorMessage;
using common::StringForFeeReason;
using common::TransactionErrorString;
using node::TransactionError;

namespace wallet {
std::vector<CRecipient> CreateRecipients(const std::vector<std::pair<CTxDestination, CAmount>>& outputs, const std::set<int>& subtract_fee_outputs)
{
    std::vector<CRecipient> recipients;
    for (size_t i = 0; i < outputs.size(); ++i) {
        const auto& [destination, amount] = outputs.at(i);
        CRecipient recipient{destination, amount, subtract_fee_outputs.contains(i)};
        recipients.push_back(recipient);
    }
    return recipients;
}

static void InterpretFeeEstimationInstructions(const UniValue& conf_target, const UniValue& estimate_mode, const UniValue& fee_rate, UniValue& options)
{
    if (options.exists("conf_target") || options.exists("estimate_mode")) {
        if (!conf_target.isNull() || !estimate_mode.isNull()) {
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Pass conf_target and estimate_mode either as arguments or in the options object, but not both");
        }
    } else {
        options.pushKV("conf_target", conf_target);
        options.pushKV("estimate_mode", estimate_mode);
    }
    if (options.exists("fee_rate")) {
        if (!fee_rate.isNull()) {
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Pass the fee_rate either as an argument, or in the options object, but not both");
        }
    } else {
        options.pushKV("fee_rate", fee_rate);
    }
    if (!options["conf_target"].isNull() && (options["estimate_mode"].isNull() || (options["estimate_mode"].get_str() == "unset"))) {
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Specify estimate_mode");
    }
}

std::set<int> InterpretSubtractFeeFromOutputInstructions(const UniValue& sffo_instructions, const std::vector<std::string>& destinations)
{
    std::set<int> sffo_set;
    if (sffo_instructions.isNull()) return sffo_set;
    if (sffo_instructions.isBool()) {
        if (sffo_instructions.get_bool()) sffo_set.insert(0);
        return sffo_set;
    }
    for (const auto& sffo : sffo_instructions.getValues()) {
        if (sffo.isStr()) {
            for (size_t i = 0; i < destinations.size(); ++i) {
                if (sffo.get_str() == destinations.at(i)) {
                    sffo_set.insert(i);
                    break;
                }
            }
        }
        if (sffo.isNum()) {
            int pos = sffo.getInt<int>();
            if (sffo_set.contains(pos))
                throw JSONRPCError(RPC_INVALID_PARAMETER, strprintf("Invalid parameter, duplicated position: %d", pos));
            if (pos < 0)
                throw JSONRPCError(RPC_INVALID_PARAMETER, strprintf("Invalid parameter, negative position: %d", pos));
            if (pos >= int(destinations.size()))
                throw JSONRPCError(RPC_INVALID_PARAMETER, strprintf("Invalid parameter, position too large: %d", pos));
            sffo_set.insert(pos);
        }
    }
    return sffo_set;
}

static UniValue FinishTransaction(const std::shared_ptr<CWallet> pwallet, const UniValue& options, const CMutableTransaction& rawTx)
{
    // Make a blank psbt
    PartiallySignedTransaction psbtx(rawTx);

    // First fill transaction with our data without signing,
    // so external signers are not asked to sign more than once.
    bool complete;
    pwallet->FillPSBT(psbtx, complete, SIGHASH_DEFAULT, /*sign=*/false, /*bip32derivs=*/true);
    const auto err{pwallet->FillPSBT(psbtx, complete, SIGHASH_DEFAULT, /*sign=*/true, /*bip32derivs=*/false)};
    if (err) {
        throw JSONRPCPSBTError(*err);
    }

    CMutableTransaction mtx;
    complete = FinalizeAndExtractPSBT(psbtx, mtx);

    UniValue result(UniValue::VOBJ);

    const bool psbt_opt_in{options.exists("psbt") && options["psbt"].get_bool()};
    bool add_to_wallet{options.exists("add_to_wallet") ? options["add_to_wallet"].get_bool() : true};
    if (psbt_opt_in || !complete || !add_to_wallet) {
        // Serialize the PSBT
        DataStream ssTx{};
        ssTx << psbtx;
        result.pushKV("psbt", EncodeBase64(ssTx.str()));
    }

    if (complete) {
        std::string hex{EncodeHexTx(CTransaction(mtx))};
        CTransactionRef tx(MakeTransactionRef(std::move(mtx)));
        result.pushKV("txid", tx->GetHash().GetHex());
        if (add_to_wallet && !psbt_opt_in) {
            pwallet->CommitTransaction(tx, {}, /*orderForm=*/{});
        } else {
            result.pushKV("hex", hex);
        }
    }
    result.pushKV("complete", complete);

    return result;
}

static void PreventOutdatedOptions(const UniValue& options)
{
    if (options.exists("feeRate")) {
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Use fee_rate (" + CURRENCY_ATOM + "/vB) instead of feeRate");
    }
    if (options.exists("changeAddress")) {
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Use change_address instead of changeAddress");
    }
    if (options.exists("changePosition")) {
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Use change_position instead of changePosition");
    }
    if (options.exists("includeWatching")) {
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Use include_watching instead of includeWatching");
    }
    if (options.exists("lockUnspents")) {
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Use lock_unspents instead of lockUnspents");
    }
    if (options.exists("subtractFeeFromOutputs")) {
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Use subtract_fee_from_outputs instead of subtractFeeFromOutputs");
    }
}

UniValue SendMoney(CWallet& wallet, const CCoinControl &coin_control, std::vector<CRecipient> &recipients, mapValue_t map_value, bool verbose)
{
    EnsureWalletIsUnlocked(wallet);

    // This function is only used by sendtoaddress and sendmany.
    // This should always try to sign, if we don't have private keys, don't try to do anything here.
    if (wallet.IsWalletFlagSet(WALLET_FLAG_DISABLE_PRIVATE_KEYS)) {
        throw JSONRPCError(RPC_WALLET_ERROR, "Error: Private keys are disabled for this wallet");
    }

    // Shuffle recipient list
    std::shuffle(recipients.begin(), recipients.end(), FastRandomContext());

    // Send
    auto res = CreateTransaction(wallet, recipients, /*change_pos=*/std::nullopt, coin_control, true);
    if (!res) {
