// Copyright (c) 2014-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <config/bitcoin-config.h> // IWYU pragma: keep

#include <crypto/sha256.h>
#include <crypto/common.h>

#include <algorithm>
#include <cassert>
#include <cstring>

#if !defined(DISABLE_OPTIMIZED_SHA256)
#include <compat/cpuid.h>

#if defined(__linux__) && defined(ENABLE_ARM_SHANI)
#include <sys/auxv.h>
#include <asm/hwcap.h>
#endif

#if defined(__APPLE__) && defined(ENABLE_ARM_SHANI)
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

#if defined(__x86_64__) || defined(__amd64__) || defined(__i386__)
namespace sha256_sse4
{
void Transform(uint32_t* s, const unsigned char* chunk, size_t blocks);
}
#endif

namespace sha256d64_sse41
{
void Transform_4way(unsigned char* out, const unsigned char* in);
}

namespace sha256d64_avx2
{
void Transform_8way(unsigned char* out, const unsigned char* in);
}

namespace sha256d64_x86_shani
{
void Transform_2way(unsigned char* out, const unsigned char* in);
}

namespace sha256_x86_shani
{
void Transform(uint32_t* s, const unsigned char* chunk, size_t blocks);
}

namespace sha256_arm_shani
{
void Transform(uint32_t* s, const unsigned char* chunk, size_t blocks);
}

namespace sha256d64_arm_shani
{
void Transform_2way(unsigned char* out, const unsigned char* in);
}
#endif // DISABLE_OPTIMIZED_SHA256

// Internal implementation code.
namespace
{
/// Internal SHA-256 implementation.
namespace sha256
{
uint32_t inline Ch(uint32_t x, uint32_t y, uint32_t z) { return z ^ (x & (y ^ z)); }
uint32_t inline Maj(uint32_t x, uint32_t y, uint32_t z) { return (x & y) | (z & (x | y)); }
uint32_t inline Sigma0(uint32_t x) { return (x >> 2 | x << 30) ^ (x >> 13 | x << 19) ^ (x >> 22 | x << 10); }
uint32_t inline Sigma1(uint32_t x) { return (x >> 6 | x << 26) ^ (x >> 11 | x << 21) ^ (x >> 25 | x << 7); }
uint32_t inline sigma0(uint32_t x) { return (x >> 7 | x << 25) ^ (x >> 18 | x << 14) ^ (x >> 3); }
uint32_t inline sigma1(uint32_t x) { return (x >> 17 | x << 15) ^ (x >> 19 | x << 13) ^ (x >> 10); }

/** One round of SHA-256. */
