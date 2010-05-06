// Copyright (c) 2018-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
//
// Based on https://github.com/noloader/SHA-Intrinsics/blob/master/sha256-x86.c,
// Written and placed in public domain by Jeffrey Walton.
// Based on code from Intel, and by Sean Gulley for the miTLS project.

#if defined(ENABLE_SSE41) && defined(ENABLE_X86_SHANI)

#include <stdint.h>
#include <immintrin.h>

#include <attributes.h>

namespace {

alignas(__m128i) const uint8_t MASK[16] = {0x03, 0x02, 0x01, 0x00, 0x07, 0x06, 0x05, 0x04, 0x0b, 0x0a, 0x09, 0x08, 0x0f, 0x0e, 0x0d, 0x0c};
alignas(__m128i) const uint8_t INIT0[16] = {0x8c, 0x68, 0x05, 0x9b, 0x7f, 0x52, 0x0e, 0x51, 0x85, 0xae, 0x67, 0xbb, 0x67, 0xe6, 0x09, 0x6a};
alignas(__m128i) const uint8_t INIT1[16] = {0x19, 0xcd, 0xe0, 0x5b, 0xab, 0xd9, 0x83, 0x1f, 0x3a, 0xf5, 0x4f, 0xa5, 0x72, 0xf3, 0x6e, 0x3c};

void ALWAYS_INLINE QuadRound(__m128i& state0, __m128i& state1, uint64_t k1, uint64_t k0)
{
    const __m128i msg = _mm_set_epi64x(k1, k0);
    state1 = _mm_sha256rnds2_epu32(state1, state0, msg);
    state0 = _mm_sha256rnds2_epu32(state0, state1, _mm_shuffle_epi32(msg, 0x0e));
}

void ALWAYS_INLINE QuadRound(__m128i& state0, __m128i& state1, __m128i m, uint64_t k1, uint64_t k0)
{
    const __m128i msg = _mm_add_epi32(m, _mm_set_epi64x(k1, k0));
    state1 = _mm_sha256rnds2_epu32(state1, state0, msg);
    state0 = _mm_sha256rnds2_epu32(state0, state1, _mm_shuffle_epi32(msg, 0x0e));
}

