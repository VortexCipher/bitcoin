// Copyright (c) 2011-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bench/bench.h>
#include <kernel/mempool_entry.h>
#include <policy/policy.h>
#include <test/util/setup_common.h>
#include <txmempool.h>


static void AddTx(const CTransactionRef& tx, const CAmount& nFee, CTxMemPool& pool) EXCLUSIVE_LOCKS_REQUIRED(cs_main, pool.cs)
{
