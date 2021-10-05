// Copyright (c) 2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_UTIL_BITDEQUE_H
#define BITCOIN_UTIL_BITDEQUE_H

#include <bitset>
#include <cstddef>
#include <deque>
#include <limits>
#include <stdexcept>
#include <tuple>

/** Class that mimics std::deque<bool>, but with std::vector<bool>'s bit packing.
 *
 * BITS_PER_WORD selects the (minimum) number of bits that are allocated at once.
 * Larger values reduce the asymptotic memory usage overhead, at the cost of
 * needing larger up-front allocations. The default is 4096 bytes.
 */
template<int BITS_PER_WORD = 4096 * 8>
class bitdeque
{
    // Internal definitions
    using word_type = std::bitset<BITS_PER_WORD>;
    using deque_type = std::deque<word_type>;
    static_assert(BITS_PER_WORD > 0);

    // Forward and friend declarations of iterator types.
    template<bool Const> class Iterator;
    template<bool Const> friend class Iterator;

    /** Iterator to a bitdeque element, const or not. */
    template<bool Const>
    class Iterator
    {
        using deque_iterator = std::conditional_t<Const, typename deque_type::const_iterator, typename deque_type::iterator>;

        deque_iterator m_it;
        int m_bitpos{0};
        Iterator(const deque_iterator& it, int bitpos) : m_it(it), m_bitpos(bitpos) {}
        friend class bitdeque;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = bool;
