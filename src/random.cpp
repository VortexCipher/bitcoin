// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <config/bitcoin-config.h> // IWYU pragma: keep

#include <random.h>

#include <compat/compat.h>
#include <compat/cpuid.h>
#include <crypto/chacha20.h>
#include <crypto/sha256.h>
#include <crypto/sha512.h>
#include <logging.h>
#include <randomenv.h>
#include <span.h>
#include <support/allocators/secure.h>
#include <support/cleanse.h>
#include <sync.h>
#include <util/time.h>

#include <array>
#include <cmath>
#include <cstdlib>
#include <optional>
#include <thread>

#ifdef WIN32
#include <windows.h>
#include <wincrypt.h>
#else
#include <fcntl.h>
#include <sys/time.h>
#endif

#if defined(HAVE_GETRANDOM) || (defined(HAVE_GETENTROPY_RAND) && defined(MAC_OSX))
#include <sys/random.h>
#endif

#ifdef HAVE_SYSCTL_ARND
#include <sys/sysctl.h>
#endif
#if defined(HAVE_STRONG_GETAUXVAL) && defined(__aarch64__)
#include <sys/auxv.h>
#endif

namespace {

/* Number of random bytes returned by GetOSRand.
 * When changing this constant make sure to change all call sites, and make
 * sure that the underlying OS APIs for all platforms support the number.
 * (many cap out at 256 bytes).
 */
static const int NUM_OS_RANDOM_BYTES = 32;


[[noreturn]] void RandFailure()
{
    LogError("Failed to read randomness, aborting\n");
    std::abort();
}

inline int64_t GetPerformanceCounter() noexcept
{
    // Read the hardware time stamp counter when available.
    // See https://en.wikipedia.org/wiki/Time_Stamp_Counter for more information.
#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
    return __rdtsc();
#elif !defined(_MSC_VER) && defined(__i386__)
    uint64_t r = 0;
    __asm__ volatile ("rdtsc" : "=A"(r)); // Constrain the r variable to the eax:edx pair.
    return r;
#elif !defined(_MSC_VER) && (defined(__x86_64__) || defined(__amd64__))
    uint64_t r1 = 0, r2 = 0;
    __asm__ volatile ("rdtsc" : "=a"(r1), "=d"(r2)); // Constrain r1 to rax and r2 to rdx.
    return (r2 << 32) | r1;
#else
