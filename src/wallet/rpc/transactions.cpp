// Copyright (c) 2011-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <core_io.h>
#include <key_io.h>
#include <policy/rbf.h>
#include <rpc/util.h>
#include <util/vector.h>
#include <wallet/receive.h>
#include <wallet/rpc/util.h>
#include <wallet/wallet.h>

using interfaces::FoundBlock;

namespace wallet {
static void WalletTxToJSON(const CWallet& wallet, const CWalletTx& wtx, UniValue& entry)
    EXCLUSIVE_LOCKS_REQUIRED(wallet.cs_wallet)
{
    interfaces::Chain& chain = wallet.chain();
    int confirms = wallet.GetTxDepthInMainChain(wtx);
    entry.pushKV("confirmations", confirms);
    if (wtx.IsCoinBase())
        entry.pushKV("generated", true);
    if (auto* conf = wtx.state<TxStateConfirmed>())
    {
        entry.pushKV("blockhash", conf->confirmed_block_hash.GetHex());
        entry.pushKV("blockheight", conf->confirmed_block_height);
        entry.pushKV("blockindex", conf->position_in_block);
        int64_t block_time;
        CHECK_NONFATAL(chain.findBlock(conf->confirmed_block_hash, FoundBlock().time(block_time)));
        entry.pushKV("blocktime", block_time);
    } else {
        entry.pushKV("trusted", CachedTxIsTrusted(wallet, wtx));
    }
    uint256 hash = wtx.GetHash();
    entry.pushKV("txid", hash.GetHex());
    entry.pushKV("wtxid", wtx.GetWitnessHash().GetHex());
    UniValue conflicts(UniValue::VARR);
    for (const uint256& conflict : wallet.GetTxConflicts(wtx))
        conflicts.push_back(conflict.GetHex());
    entry.pushKV("walletconflicts", std::move(conflicts));
    UniValue mempool_conflicts(UniValue::VARR);
    for (const Txid& mempool_conflict : wtx.mempool_conflicts)
        mempool_conflicts.push_back(mempool_conflict.GetHex());
    entry.pushKV("mempoolconflicts", std::move(mempool_conflicts));
    entry.pushKV("time", wtx.GetTxTime());
    entry.pushKV("timereceived", int64_t{wtx.nTimeReceived});

    // Add opt-in RBF status
    std::string rbfStatus = "no";
    if (confirms <= 0) {
        RBFTransactionState rbfState = chain.isRBFOptIn(*wtx.tx);
        if (rbfState == RBFTransactionState::UNKNOWN)
            rbfStatus = "unknown";
        else if (rbfState == RBFTransactionState::REPLACEABLE_BIP125)
            rbfStatus = "yes";
    }
    entry.pushKV("bip125-replaceable", rbfStatus);

    for (const std::pair<const std::string, std::string>& item : wtx.mapValue)
        entry.pushKV(item.first, item.second);
}

struct tallyitem
{
    CAmount nAmount{0};
    int nConf{std::numeric_limits<int>::max()};
    std::vector<uint256> txids;
    bool fIsWatchonly{false};
    tallyitem() = default;
};

static UniValue ListReceived(const CWallet& wallet, const UniValue& params, const bool by_label, const bool include_immature_coinbase) EXCLUSIVE_LOCKS_REQUIRED(wallet.cs_wallet)
{
    // Minimum confirmations
    int nMinDepth = 1;
    if (!params[0].isNull())
        nMinDepth = params[0].getInt<int>();

    // Whether to include empty labels
    bool fIncludeEmpty = false;
    if (!params[1].isNull())
        fIncludeEmpty = params[1].get_bool();

    isminefilter filter = ISMINE_SPENDABLE;

    if (ParseIncludeWatchonly(params[2], wallet)) {
        filter |= ISMINE_WATCH_ONLY;
    }

    std::optional<CTxDestination> filtered_address{std::nullopt};
    if (!by_label && !params[3].isNull() && !params[3].get_str().empty()) {
        if (!IsValidDestinationString(params[3].get_str())) {
            throw JSONRPCError(RPC_WALLET_ERROR, "address_filter parameter was invalid");
        }
