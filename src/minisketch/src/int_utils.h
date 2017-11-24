/**********************************************************************
 * Copyright (c) 2018 Pieter Wuille, Greg Maxwell, Gleb Naumenko      *
 * Distributed under the MIT software license, see the accompanying   *
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.*
 **********************************************************************/

#ifndef _MINISKETCH_INT_UTILS_H_
#define _MINISKETCH_INT_UTILS_H_

#include <stdint.h>
#include <stdlib.h>

#include <limits>
#include <algorithm>
#include <type_traits>

#if defined(__cpp_lib_int_pow2) && __cpp_lib_int_pow2 >= 202002L
#  include <bit>
#elif defined(_MSC_VER)
#  include <intrin.h>
#endif

template<int bits>
static constexpr inline uint64_t Rot(uint64_t x) { return (x << bits) | (x >> (64 - bits)); }

static inline void SipHashRound(uint64_t& v0, uint64_t& v1, uint64_t& v2, uint64_t& v3) {
    v0 += v1; v1 = Rot<13>(v1); v1 ^= v0;
    v0 = Rot<32>(v0);
    v2 += v3; v3 = Rot<16>(v3); v3 ^= v2;
    v0 += v3; v3 = Rot<21>(v3); v3 ^= v0;
    v2 += v1; v1 = Rot<17>(v1); v1 ^= v2;
