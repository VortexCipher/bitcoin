// Copyright (c) 2020-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <crypto/common.h>
#include <crypto/sha256.h>
#include <crypto/siphash.h>
#include <primitives/transaction.h>
#include <test/fuzz/fuzz.h>
#include <txrequest.h>

#include <bitset>
#include <cstdint>
#include <queue>
#include <vector>

namespace {

constexpr int MAX_TXHASHES = 16;
constexpr int MAX_PEERS = 16;

//! Randomly generated GenTxids used in this test (length is MAX_TXHASHES).
uint256 TXHASHES[MAX_TXHASHES];

//! Precomputed random durations (positive and negative, each ~exponentially distributed).
std::chrono::microseconds DELAYS[256];

struct Initializer
{
    Initializer()
    {
        for (uint8_t txhash = 0; txhash < MAX_TXHASHES; txhash += 1) {
            CSHA256().Write(&txhash, 1).Finalize(TXHASHES[txhash].begin());
        }
        int i = 0;
        // DELAYS[N] for N=0..15 is just N microseconds.
        for (; i < 16; ++i) {
