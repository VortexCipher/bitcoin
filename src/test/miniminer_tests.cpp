// Copyright (c) 2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include <node/mini_miner.h>
#include <random.h>
#include <txmempool.h>
#include <util/time.h>

#include <test/util/setup_common.h>
#include <test/util/txmempool.h>

#include <boost/test/unit_test.hpp>
#include <optional>
#include <vector>

BOOST_FIXTURE_TEST_SUITE(miniminer_tests, TestingSetup)

const CAmount low_fee{CENT/2000}; // 500 ṩ
const CAmount med_fee{CENT/200}; // 5000 ṩ
const CAmount high_fee{CENT/10}; // 100_000 ṩ


static inline CTransactionRef make_tx(const std::vector<COutPoint>& inputs, size_t num_outputs)
{
    CMutableTransaction tx = CMutableTransaction();
    tx.vin.resize(inputs.size());
    tx.vout.resize(num_outputs);
    for (size_t i = 0; i < inputs.size(); ++i) {
        tx.vin[i].prevout = inputs[i];
    }
    for (size_t i = 0; i < num_outputs; ++i) {
        tx.vout[i].scriptPubKey = CScript() << OP_11 << OP_EQUAL;
        // The actual input and output values of these transactions don't really
        // matter, since all accounting will use the entries' cached fees.
        tx.vout[i].nValue = COIN;
    }
    return MakeTransactionRef(tx);
}

static inline bool sanity_check(const std::vector<CTransactionRef>& transactions,
                                const std::map<COutPoint, CAmount>& bumpfees)
{
    // No negative bumpfees.
    for (const auto& [outpoint, fee] : bumpfees) {
        if (fee < 0) return false;
        if (fee == 0) continue;
        auto outpoint_ = outpoint; // structured bindings can't be captured in C++17, so we need to use a variable
        const bool found = std::any_of(transactions.cbegin(), transactions.cend(), [&](const auto& tx) {
            return outpoint_.hash == tx->GetHash() && outpoint_.n < tx->vout.size();
        });
        if (!found) return false;
    }
    for (const auto& tx : transactions) {
        // If tx has multiple outputs, they must all have the same bumpfee (if they exist).
        if (tx->vout.size() > 1) {
            std::set<CAmount> distinct_bumpfees;
            for (size_t i{0}; i < tx->vout.size(); ++i) {
                const auto bumpfee = bumpfees.find(COutPoint{tx->GetHash(), static_cast<uint32_t>(i)});
                if (bumpfee != bumpfees.end()) distinct_bumpfees.insert(bumpfee->second);
            }
            if (distinct_bumpfees.size() > 1) return false;
        }
    }
    return true;
}

template <typename Key, typename Value>
Value Find(const std::map<Key, Value>& map, const Key& key)
{
    auto it = map.find(key);
    BOOST_CHECK_MESSAGE(it != map.end(), strprintf("Cannot find %s", key.ToString()));
