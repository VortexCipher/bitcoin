// Copyright (c) 2011-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <arith_uint256.h>
#include <test/util/setup_common.h>
#include <uint256.h>

#include <boost/test/unit_test.hpp>

#include <cmath>
#include <cstdint>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

BOOST_AUTO_TEST_SUITE(arith_uint256_tests)

/// Convert vector to arith_uint256, via uint256 blob
static inline arith_uint256 arith_uint256V(const std::vector<unsigned char>& vch)
{
    return UintToArith256(uint256(vch));
}
// Takes a number written in hex (with most significant digits first).
static inline arith_uint256 arith_uint256S(std::string_view str) { return UintToArith256(uint256S(str)); }

const unsigned char R1Array[] =
    "\x9c\x52\x4a\xdb\xcf\x56\x11\x12\x2b\x29\x12\x5e\x5d\x35\xd2\xd2"
    "\x22\x81\xaa\xb5\x33\xf0\x08\x32\xd5\x56\xb1\xf9\xea\xe5\x1d\x7d";
const char R1ArrayHex[] = "7D1DE5EAF9B156D53208F033B5AA8122D2d2355d5e12292b121156cfdb4a529c";
const double R1Ldouble = 0.4887374590559308955; // R1L equals roughly R1Ldouble * 2^256
const arith_uint256 R1L = arith_uint256V(std::vector<unsigned char>(R1Array,R1Array+32));
const uint64_t R1LLow64 = 0x121156cfdb4a529cULL;

const unsigned char R2Array[] =
    "\x70\x32\x1d\x7c\x47\xa5\x6b\x40\x26\x7e\x0a\xc3\xa6\x9c\xb6\xbf"
    "\x13\x30\x47\xa3\x19\x2d\xda\x71\x49\x13\x72\xf0\xb4\xca\x81\xd7";
const arith_uint256 R2L = arith_uint256V(std::vector<unsigned char>(R2Array,R2Array+32));

const char R1LplusR2L[] = "549FB09FEA236A1EA3E31D4D58F1B1369288D204211CA751527CFC175767850C";

const unsigned char ZeroArray[] =
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
const arith_uint256 ZeroL = arith_uint256V(std::vector<unsigned char>(ZeroArray,ZeroArray+32));

const unsigned char OneArray[] =
    "\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
const arith_uint256 OneL = arith_uint256V(std::vector<unsigned char>(OneArray,OneArray+32));

const unsigned char MaxArray[] =
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff";
const arith_uint256 MaxL = arith_uint256V(std::vector<unsigned char>(MaxArray,MaxArray+32));
