// Copyright (c) 2016-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_SUPPORT_LOCKEDPOOL_H
#define BITCOIN_SUPPORT_LOCKEDPOOL_H

#include <cstddef>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <unordered_map>

/**
 * OS-dependent allocation and deallocation of locked/pinned memory pages.
 * Abstract base class.
 */
class LockedPageAllocator
{
public:
    virtual ~LockedPageAllocator() = default;
    /** Allocate and lock memory pages.
     * If len is not a multiple of the system page size, it is rounded up.
