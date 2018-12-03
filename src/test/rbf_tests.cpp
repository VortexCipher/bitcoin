// Copyright (c) 2021-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include <common/system.h>
#include <policy/rbf.h>
#include <random.h>
#include <test/util/txmempool.h>
#include <txmempool.h>
#include <util/time.h>

#include <test/util/setup_common.h>

#include <boost/test/unit_test.hpp>
#include <optional>
#include <vector>

BOOST_FIXTURE_TEST_SUITE(rbf_tests, TestingSetup)

static inline CTransactionRef make_tx(const std::vector<CTransactionRef>& inputs,
                                      const std::vector<CAmount>& output_values)
{
    CMutableTransaction tx = CMutableTransaction();
    tx.vin.resize(inputs.size());
    tx.vout.resize(output_values.size());
    for (size_t i = 0; i < inputs.size(); ++i) {
        tx.vin[i].prevout.hash = inputs[i]->GetHash();
        tx.vin[i].prevout.n = 0;
        // Add a witness so wtxid != txid
        CScriptWitness witness;
        witness.stack.emplace_back(i + 10);
        tx.vin[i].scriptWitness = witness;
    }
    for (size_t i = 0; i < output_values.size(); ++i) {
        tx.vout[i].scriptPubKey = CScript() << OP_11 << OP_EQUAL;
        tx.vout[i].nValue = output_values[i];
    }
    return MakeTransactionRef(tx);
}

// Make two child transactions from parent (which must have at least 2 outputs).
// Each tx will have the same outputs, using the amounts specified in output_values.
static inline std::pair<CTransactionRef, CTransactionRef> make_two_siblings(const CTransactionRef parent,
                                      const std::vector<CAmount>& output_values)
{
    assert(parent->vout.size() >= 2);

    // First tx takes first parent output
    CMutableTransaction tx1 = CMutableTransaction();
    tx1.vin.resize(1);
    tx1.vout.resize(output_values.size());

    tx1.vin[0].prevout.hash = parent->GetHash();
    tx1.vin[0].prevout.n = 0;
    // Add a witness so wtxid != txid
    CScriptWitness witness;
    witness.stack.emplace_back(10);
    tx1.vin[0].scriptWitness = witness;

    for (size_t i = 0; i < output_values.size(); ++i) {
        tx1.vout[i].scriptPubKey = CScript() << OP_11 << OP_EQUAL;
        tx1.vout[i].nValue = output_values[i];
    }
