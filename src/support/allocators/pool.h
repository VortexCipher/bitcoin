// Copyright (c) 2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_SUPPORT_ALLOCATORS_POOL_H
#define BITCOIN_SUPPORT_ALLOCATORS_POOL_H

#include <array>
#include <cassert>
#include <cstddef>
#include <list>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

/**
 * A memory resource similar to std::pmr::unsynchronized_pool_resource, but
 * optimized for node-based containers. It has the following properties:
 *
 * * Owns the allocated memory and frees it on destruction, even when deallocate
 *   has not been called on the allocated blocks.
 *
 * * Consists of a number of pools, each one for a different block size.
 *   Each pool holds blocks of uniform size in a freelist.
 *
 * * Exhausting memory in a freelist causes a new allocation of a fixed size chunk.
 *   This chunk is used to carve out blocks.
 *
 * * Block sizes or alignments that can not be served by the pools are allocated
 *   and deallocated by operator new().
 *
 * PoolResource is not thread-safe. It is intended to be used by PoolAllocator.
 *
