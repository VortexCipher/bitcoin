// Copyright (c) 2011-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <addresstype.h>
#include <coins.h>
#include <common/system.h>
#include <consensus/consensus.h>
#include <consensus/merkle.h>
#include <consensus/tx_verify.h>
#include <node/miner.h>
#include <policy/policy.h>
#include <test/util/random.h>
#include <test/util/txmempool.h>
#include <txmempool.h>
#include <uint256.h>
#include <util/check.h>
#include <util/strencodings.h>
#include <util/time.h>
#include <util/translation.h>
#include <validation.h>
#include <versionbits.h>

#include <test/util/setup_common.h>

#include <memory>

#include <boost/test/unit_test.hpp>

using node::BlockAssembler;
using node::CBlockTemplate;

namespace miner_tests {
struct MinerTestingSetup : public TestingSetup {
    void TestPackageSelection(const CScript& scriptPubKey, const std::vector<CTransactionRef>& txFirst) EXCLUSIVE_LOCKS_REQUIRED(::cs_main);
    void TestBasicMining(const CScript& scriptPubKey, const std::vector<CTransactionRef>& txFirst, int baseheight) EXCLUSIVE_LOCKS_REQUIRED(::cs_main);
    void TestPrioritisedMining(const CScript& scriptPubKey, const std::vector<CTransactionRef>& txFirst) EXCLUSIVE_LOCKS_REQUIRED(::cs_main);
    bool TestSequenceLocks(const CTransaction& tx, CTxMemPool& tx_mempool) EXCLUSIVE_LOCKS_REQUIRED(::cs_main)
    {
        CCoinsViewMemPool view_mempool{&m_node.chainman->ActiveChainstate().CoinsTip(), tx_mempool};
        CBlockIndex* tip{m_node.chainman->ActiveChain().Tip()};
        const std::optional<LockPoints> lock_points{CalculateLockPointsAtTip(tip, view_mempool, tx)};
        return lock_points.has_value() && CheckSequenceLocksAtTip(tip, *lock_points);
    }
    CTxMemPool& MakeMempool()
    {
        // Delete the previous mempool to ensure with valgrind that the old
        // pointer is not accessed, when the new one should be accessed
        // instead.
        m_node.mempool.reset();
        bilingual_str error;
        m_node.mempool = std::make_unique<CTxMemPool>(MemPoolOptionsForTest(m_node), error);
        Assert(error.empty());
        return *m_node.mempool;
    }
    BlockAssembler AssemblerForTest(CTxMemPool& tx_mempool);
};
} // namespace miner_tests

BOOST_FIXTURE_TEST_SUITE(miner_tests, MinerTestingSetup)

static CFeeRate blockMinFeeRate = CFeeRate(DEFAULT_BLOCK_MIN_TX_FEE);

BlockAssembler MinerTestingSetup::AssemblerForTest(CTxMemPool& tx_mempool)
{
    BlockAssembler::Options options;
