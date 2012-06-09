// Copyright (c) The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_UTIL_BITSET_H
#define BITCOIN_UTIL_BITSET_H

#include <util/check.h>

#include <array>
#include <bit>
#include <cstdint>
#include <limits>
#include <type_traits>

/* This file provides data types similar to std::bitset, but adds the following functionality:
 *
 * - Efficient iteration over all set bits (compatible with range-based for loops).
 * - Efficient search for the first and last set bit (First() and Last()).
 * - Efficient set subtraction: (a - b) implements "a and not b".
 * - Efficient non-strict subset/superset testing: IsSubsetOf() and IsSupersetOf().
 * - Efficient set overlap testing: a.Overlaps(b)
 * - Efficient construction of set containing 0..N-1 (S::Fill).
 * - Efficient construction of a single set (S::Singleton).
 * - Construction from initializer lists.
 *
 * Other differences:
 * - BitSet<N> is a bitset that supports at least N elements, but may support more (Size() reports
 *   the actual number). Because the actual number is unpredictable, there are no operations that
 *   affect all positions (like std::bitset's operator~, flip(), or all()).
 * - Various other unimplemented features.
 */

namespace bitset_detail {

/** Count the number of bits set in an unsigned integer type. */
template<typename I>
unsigned inline constexpr PopCount(I v)
{
    static_assert(std::is_integral_v<I> && std::is_unsigned_v<I> && std::numeric_limits<I>::radix == 2);
    constexpr auto BITS = std::numeric_limits<I>::digits;
    // Algorithms from https://en.wikipedia.org/wiki/Hamming_weight#Efficient_implementation.
    // These seem to be faster than std::popcount when compiling for non-SSE4 on x86_64.
    if constexpr (BITS <= 32) {
        v -= (v >> 1) & 0x55555555;
        v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
        v = (v + (v >> 4)) & 0x0f0f0f0f;
        if constexpr (BITS > 8) v += v >> 8;
        if constexpr (BITS > 16) v += v >> 16;
        return v & 0x3f;
    } else {
        static_assert(BITS <= 64);
