// Copyright (c) 2011-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <core_io.h>
#include <hash.h>
#include <key_io.h>
#include <rpc/util.h>
#include <script/script.h>
#include <util/moneystr.h>
#include <wallet/coincontrol.h>
#include <wallet/receive.h>
#include <wallet/rpc/util.h>
#include <wallet/spend.h>
#include <wallet/wallet.h>

#include <univalue.h>


namespace wallet {
static CAmount GetReceived(const CWallet& wallet, const UniValue& params, bool by_label) EXCLUSIVE_LOCKS_REQUIRED(wallet.cs_wallet)
{
    std::vector<CTxDestination> addresses;
    if (by_label) {
        // Get the set of addresses assigned to label
        addresses = wallet.ListAddrBookAddresses(CWallet::AddrBookFilter{LabelFromValue(params[0])});
        if (addresses.empty()) throw JSONRPCError(RPC_WALLET_ERROR, "Label not found in wallet");
    } else {
        // Get the address
        CTxDestination dest = DecodeDestination(params[0].get_str());
        if (!IsValidDestination(dest)) {
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid Bitcoin address");
        }
        addresses.emplace_back(dest);
    }

    // Filter by own scripts only
    std::set<CScript> output_scripts;
    for (const auto& address : addresses) {
        auto output_script{GetScriptForDestination(address)};
        if (wallet.IsMine(output_script)) {
            output_scripts.insert(output_script);
        }
    }

    if (output_scripts.empty()) {
        throw JSONRPCError(RPC_WALLET_ERROR, "Address not found in wallet");
    }

    // Minimum confirmations
    int min_depth = 1;
    if (!params[1].isNull())
        min_depth = params[1].getInt<int>();

    const bool include_immature_coinbase{params[2].isNull() ? false : params[2].get_bool()};

    // Tally
    CAmount amount = 0;
    for (const std::pair<const uint256, CWalletTx>& wtx_pair : wallet.mapWallet) {
        const CWalletTx& wtx = wtx_pair.second;
        int depth{wallet.GetTxDepthInMainChain(wtx)};
        if (depth < min_depth
            // Coinbase with less than 1 confirmation is no longer in the main chain
            || (wtx.IsCoinBase() && (depth < 1))
            || (wallet.IsTxImmatureCoinBase(wtx) && !include_immature_coinbase))
        {
            continue;
        }

        for (const CTxOut& txout : wtx.tx->vout) {
            if (output_scripts.count(txout.scriptPubKey) > 0) {
                amount += txout.nValue;
            }
