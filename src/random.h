// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_RANDOM_H
#define BITCOIN_RANDOM_H

#include <crypto/chacha20.h>
#include <crypto/common.h>
#include <span.h>
#include <uint256.h>
#include <util/check.h>

#include <bit>
#include <cassert>
#include <chrono>
#include <concepts>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>

/**
 * Overall design of the RNG and entropy sources.
 *
 * We maintain a single global 256-bit RNG state for all high-quality randomness.
 * The following (classes of) functions interact with that state by mixing in new
 * entropy, and optionally extracting random output from it:
 *
 * - GetRandBytes, GetRandHash, GetRandDur, as well as construction of FastRandomContext
 *   objects, perform 'fast' seeding, consisting of mixing in:
 *   - A stack pointer (indirectly committing to calling thread and call stack)
 *   - A high-precision timestamp (rdtsc when available, c++ high_resolution_clock otherwise)
 *   - 64 bits from the hardware RNG (rdrand) when available.
 *   These entropy sources are very fast, and only designed to protect against situations
 *   where a VM state restore/copy results in multiple systems with the same randomness.
 *   FastRandomContext on the other hand does not protect against this once created, but
 *   is even faster (and acceptable to use inside tight loops).
 *
 * - The GetStrongRandBytes() function performs 'slow' seeding, including everything
 *   that fast seeding includes, but additionally:
 *   - OS entropy (/dev/urandom, getrandom(), ...). The application will terminate if
 *     this entropy source fails.
 *   - Another high-precision timestamp (indirectly committing to a benchmark of all the
 *     previous sources).
