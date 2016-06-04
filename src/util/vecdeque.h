// Copyright (c) The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_UTIL_VECDEQUE_H
#define BITCOIN_UTIL_VECDEQUE_H

#include <util/check.h>

#include <cstring>
#include <memory>
#include <type_traits>

/** Data structure largely mimicking std::deque, but using single preallocated ring buffer.
 *
 * - More efficient and better memory locality than std::deque.
 * - Most operations ({push_,pop_,emplace_,}{front,back}(), operator[], ...) are O(1),
 *   unless reallocation is needed (in which case they are O(n)).
 * - Supports reserve(), capacity(), shrink_to_fit() like vectors.
 * - No iterator support.
 * - Data is not stored in a single contiguous block, so no data().
 */
template<typename T>
class VecDeque
{
    /** Pointer to allocated memory. Can contain constructed and uninitialized T objects. */
    T* m_buffer{nullptr};
    /** m_buffer + m_offset points to first object in queue. m_offset = 0 if m_capacity is 0;
     *  otherwise 0 <= m_offset < m_capacity. */
    size_t m_offset{0};
    /** Number of objects in the container. 0 <= m_size <= m_capacity. */
