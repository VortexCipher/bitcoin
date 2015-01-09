// Copyright (c) 2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <policy/feerate.h>
#include <policy/policy.h>
#include <primitives/transaction.h>
#include <test/fuzz/FuzzedDataProvider.h>
#include <test/fuzz/fuzz.h>
#include <test/fuzz/util.h>
#include <test/util/setup_common.h>
#include <wallet/coinselection.h>

#include <numeric>
#include <vector>

namespace wallet {

static void AddCoin(const CAmount& value, int n_input, int n_input_bytes, int locktime, std::vector<COutput>& coins, CFeeRate fee_rate)
{
    CMutableTransaction tx;
    tx.vout.resize(n_input + 1);
    tx.vout[n_input].nValue = value;
    tx.nLockTime = locktime; // all transactions get different hashes
    coins.emplace_back(COutPoint(tx.GetHash(), n_input), tx.vout.at(n_input), /*depth=*/0, n_input_bytes, /*spendable=*/true, /*solvable=*/true, /*safe=*/true, /*time=*/0, /*from_me=*/true, fee_rate);
}

// Randomly distribute coins to instances of OutputGroup
static void GroupCoins(FuzzedDataProvider& fuzzed_data_provider, const std::vector<COutput>& coins, const CoinSelectionParams& coin_params, bool positive_only, std::vector<OutputGroup>& output_groups)
{
    auto output_group = OutputGroup(coin_params);
    bool valid_outputgroup{false};
    for (auto& coin : coins) {
