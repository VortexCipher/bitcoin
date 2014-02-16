// Copyright (c) 2017-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <addresstype.h>
#include <blockfilter.h>
#include <chainparams.h>
#include <consensus/merkle.h>
#include <consensus/validation.h>
#include <index/blockfilterindex.h>
#include <interfaces/chain.h>
#include <node/miner.h>
#include <pow.h>
#include <test/util/blockfilter.h>
#include <test/util/index.h>
#include <test/util/setup_common.h>
#include <validation.h>

#include <boost/test/unit_test.hpp>

using node::BlockAssembler;
using node::BlockManager;
using node::CBlockTemplate;

BOOST_AUTO_TEST_SUITE(blockfilter_index_tests)

struct BuildChainTestingSetup : public TestChain100Setup {
    CBlock CreateBlock(const CBlockIndex* prev, const std::vector<CMutableTransaction>& txns, const CScript& scriptPubKey);
    bool BuildChain(const CBlockIndex* pindex, const CScript& coinbase_script_pub_key, size_t length, std::vector<std::shared_ptr<CBlock>>& chain);
};

