// Copyright (c) 2023 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <coins.h>
#include <crypto/sha256.h>
#include <primitives/transaction.h>
#include <test/fuzz/fuzz.h>
#include <test/fuzz/FuzzedDataProvider.h>
#include <test/fuzz/util.h>

#include <assert.h>
#include <optional>
#include <memory>
#include <stdint.h>
#include <vector>

namespace {

/** Number of distinct COutPoint values used in this test. */
constexpr uint32_t NUM_OUTPOINTS = 256;
/** Number of distinct Coin values used in this test (ignoring nHeight). */
constexpr uint32_t NUM_COINS = 256;
/** Maximum number CCoinsViewCache objects used in this test. */
constexpr uint32_t MAX_CACHES = 4;
/** Data type large enough to hold NUM_COINS-1. */
using coinidx_type = uint8_t;

struct PrecomputedData
{
    //! Randomly generated COutPoint values.
    COutPoint outpoints[NUM_OUTPOINTS];

    //! Randomly generated Coin values.
    Coin coins[NUM_COINS];

    PrecomputedData()
    {
        static const uint8_t PREFIX_O[1] = {'o'}; /** Hash prefix for outpoint hashes. */
        static const uint8_t PREFIX_S[1] = {'s'}; /** Hash prefix for coins scriptPubKeys. */
        static const uint8_t PREFIX_M[1] = {'m'}; /** Hash prefix for coins nValue/fCoinBase. */

        for (uint32_t i = 0; i < NUM_OUTPOINTS; ++i) {
            uint32_t idx = (i * 1200U) >> 12; /* Map 3 or 4 entries to same txid. */
            const uint8_t ser[4] = {uint8_t(idx), uint8_t(idx >> 8), uint8_t(idx >> 16), uint8_t(idx >> 24)};
            uint256 txid;
            CSHA256().Write(PREFIX_O, 1).Write(ser, sizeof(ser)).Finalize(txid.begin());
            outpoints[i].hash = Txid::FromUint256(txid);
