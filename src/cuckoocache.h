// Copyright (c) 2016 Jeremy Rubin
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CUCKOOCACHE_H
#define BITCOIN_CUCKOOCACHE_H

#include <util/fastrange.h>

#include <algorithm> // std::find
#include <array>
#include <atomic>
#include <cmath>
#include <cstring>
#include <limits>
#include <memory>
#include <utility>
#include <vector>


/** High-performance cache primitives.
 *
 * Summary:
 *
 * 1. @ref bit_packed_atomic_flags is bit-packed atomic flags for garbage collection
 *
 * 2. @ref cache is a cache which is performant in memory usage and lookup speed. It
 * is lockfree for erase operations. Elements are lazily erased on the next insert.
 */
namespace CuckooCache
{
/** @ref bit_packed_atomic_flags implements a container for garbage collection flags
 * that is only thread unsafe on calls to setup. This class bit-packs collection
 * flags for memory efficiency.
 *
 * All operations are `std::memory_order_relaxed` so external mechanisms must
 * ensure that writes and reads are properly synchronized.
 *
 * On setup(n), all bits up to `n` are marked as collected.
 *
 * Under the hood, because it is an 8-bit type, it makes sense to use a multiple
 * of 8 for setup, but it will be safe if that is not the case as well.
 */
class bit_packed_atomic_flags
{
    std::unique_ptr<std::atomic<uint8_t>[]> mem;

public:
