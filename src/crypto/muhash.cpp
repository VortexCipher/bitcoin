// Copyright (c) 2017-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <crypto/muhash.h>

#include <crypto/chacha20.h>
#include <crypto/common.h>
#include <hash.h>

#include <cassert>
#include <cstdio>
#include <limits>

namespace {

using limb_t = Num3072::limb_t;
using double_limb_t = Num3072::double_limb_t;
constexpr int LIMB_SIZE = Num3072::LIMB_SIZE;
/** 2^3072 - 1103717, the largest 3072-bit safe prime number, is used as the modulus. */
constexpr limb_t MAX_PRIME_DIFF = 1103717;

/** Extract the lowest limb of [c0,c1,c2] into n, and left shift the number by 1 limb. */
inline void extract3(limb_t& c0, limb_t& c1, limb_t& c2, limb_t& n)
{
    n = c0;
    c0 = c1;
    c1 = c2;
    c2 = 0;
}

/** [c0,c1] = a * b */
inline void mul(limb_t& c0, limb_t& c1, const limb_t& a, const limb_t& b)
{
