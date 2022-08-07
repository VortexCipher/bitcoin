// Copyright (c) 2023 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <node/mini_miner.h>

#include <boost/multi_index/detail/hash_index_iterator.hpp>
#include <boost/operators.hpp>
#include <consensus/amount.h>
#include <policy/feerate.h>
#include <primitives/transaction.h>
#include <sync.h>
#include <txmempool.h>
#include <uint256.h>
#include <util/check.h>

#include <algorithm>
#include <numeric>
#include <utility>

namespace node {

MiniMiner::MiniMiner(const CTxMemPool& mempool, const std::vector<COutPoint>& outpoints)
{
    LOCK(mempool.cs);
    // Find which outpoints to calculate bump fees for.
    // Anything that's spent by the mempool is to-be-replaced
    // Anything otherwise unavailable just has a bump fee of 0
    for (const auto& outpoint : outpoints) {
        if (!mempool.exists(GenTxid::Txid(outpoint.hash))) {
            // This UTXO is either confirmed or not yet submitted to mempool.
            // If it's confirmed, no bump fee is required.
            // If it's not yet submitted, we have no information, so return 0.
            m_bump_fees.emplace(outpoint, 0);
            continue;
        }

        // UXTO is created by transaction in mempool, add to map.
        // Note: This will either create a missing entry or add the outpoint to an existing entry
        m_requested_outpoints_by_txid[outpoint.hash].push_back(outpoint);

        if (const auto ptx{mempool.GetConflictTx(outpoint)}) {
            // This outpoint is already being spent by another transaction in the mempool. We
