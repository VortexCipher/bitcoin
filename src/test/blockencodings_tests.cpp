// Copyright (c) 2011-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <blockencodings.h>
#include <chainparams.h>
#include <consensus/merkle.h>
#include <pow.h>
#include <streams.h>
#include <test/util/random.h>
#include <test/util/txmempool.h>

#include <test/util/setup_common.h>

#include <boost/test/unit_test.hpp>

const std::vector<CTransactionRef> empty_extra_txn;

BOOST_FIXTURE_TEST_SUITE(blockencodings_tests, RegTestingSetup)

static CMutableTransaction BuildTransactionTestCase() {
    CMutableTransaction tx;
    tx.vin.resize(1);
    tx.vin[0].scriptSig.resize(10);
    tx.vout.resize(1);
    tx.vout[0].nValue = 42;
    return tx;
}

static CBlock BuildBlockTestCase(FastRandomContext& ctx) {
    CBlock block;
    CMutableTransaction tx = BuildTransactionTestCase();

    block.vtx.resize(3);
    block.vtx[0] = MakeTransactionRef(tx);
    block.nVersion = 42;
    block.hashPrevBlock = ctx.rand256();
    block.nBits = 0x207fffff;

    tx.vin[0].prevout.hash = Txid::FromUint256(ctx.rand256());
    tx.vin[0].prevout.n = 0;
    block.vtx[1] = MakeTransactionRef(tx);
