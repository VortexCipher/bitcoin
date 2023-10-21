// Copyright (c) 2016-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <support/lockedpool.h>
#include <support/cleanse.h>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/mman.h> // for mmap
#include <sys/resource.h> // for getrlimit
#include <limits.h> // for PAGESIZE
#include <unistd.h> // for sysconf
#endif

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>
#ifdef ARENA_DEBUG
#include <iomanip>
#include <iostream>
#endif

LockedPoolManager* LockedPoolManager::_instance = nullptr;

/*******************************************************************************/
// Utilities
//
/** Align up to power of 2 */
static inline size_t align_up(size_t x, size_t align)
{
    return (x + align - 1) & ~(align - 1);
}

/*******************************************************************************/
// Implementation: Arena

Arena::Arena(void *base_in, size_t size_in, size_t alignment_in):
