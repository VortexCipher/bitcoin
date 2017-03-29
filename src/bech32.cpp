// Copyright (c) 2017, 2021 Pieter Wuille
// Copyright (c) 2021-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bech32.h>
#include <util/vector.h>

#include <array>
#include <assert.h>
#include <numeric>
#include <optional>

namespace bech32
{

namespace
{

typedef std::vector<uint8_t> data;

/** The Bech32 and Bech32m character set for encoding. */
const char* CHARSET = "qpzry9x8gf2tvdw0s3jn54khce6mua7l";

/** The Bech32 and Bech32m character set for decoding. */
const int8_t CHARSET_REV[128] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    15, -1, 10, 17, 21, 20, 26, 30,  7,  5, -1, -1, -1, -1, -1, -1,
    -1, 29, -1, 24, 13, 25,  9,  8, 23, -1, 18, 22, 31, 27, 19, -1,
     1,  0,  3, 16, 11, 28, 12, 14,  6,  4,  2, -1, -1, -1, -1, -1,
    -1, 29, -1, 24, 13, 25,  9,  8, 23, -1, 18, 22, 31, 27, 19, -1,
     1,  0,  3, 16, 11, 28, 12, 14,  6,  4,  2, -1, -1, -1, -1, -1
};

/** We work with the finite field GF(1024) defined as a degree 2 extension of the base field GF(32)
 * The defining polynomial of the extension is x^2 + 9x + 23.
 * Let (e) be a root of this defining polynomial. Then (e) is a primitive element of GF(1024),
 * that is, a generator of the field. Every non-zero element of the field can then be represented
 * as (e)^k for some power k.
 * The array GF1024_EXP contains all these powers of (e) - GF1024_EXP[k] = (e)^k in GF(1024).
 * Conversely, GF1024_LOG contains the discrete logarithms of these powers, so
 * GF1024_LOG[GF1024_EXP[k]] == k.
 * The following function generates the two tables GF1024_EXP and GF1024_LOG as constexprs. */
constexpr std::pair<std::array<int16_t, 1023>, std::array<int16_t, 1024>> GenerateGFTables()
{
    // Build table for GF(32).
    // We use these tables to perform arithmetic in GF(32) below, when constructing the
    // tables for GF(1024).
    std::array<int8_t, 31> GF32_EXP{};
    std::array<int8_t, 32> GF32_LOG{};

    // fmod encodes the defining polynomial of GF(32) over GF(2), x^5 + x^3 + 1.
    // Because coefficients in GF(2) are binary digits, the coefficients are packed as 101001.
    const int fmod = 41;

