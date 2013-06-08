// Copyright (c) 2018-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifdef ENABLE_SSE41

#include <stdint.h>
#include <immintrin.h>

#include <attributes.h>
#include <crypto/common.h>

namespace sha256d64_sse41 {
namespace {

__m128i inline K(uint32_t x) { return _mm_set1_epi32(x); }

__m128i inline Add(__m128i x, __m128i y) { return _mm_add_epi32(x, y); }
__m128i inline Add(__m128i x, __m128i y, __m128i z) { return Add(Add(x, y), z); }
__m128i inline Add(__m128i x, __m128i y, __m128i z, __m128i w) { return Add(Add(x, y), Add(z, w)); }
__m128i inline Add(__m128i x, __m128i y, __m128i z, __m128i w, __m128i v) { return Add(Add(x, y, z), Add(w, v)); }
__m128i inline Inc(__m128i& x, __m128i y) { x = Add(x, y); return x; }
__m128i inline Inc(__m128i& x, __m128i y, __m128i z) { x = Add(x, y, z); return x; }
__m128i inline Inc(__m128i& x, __m128i y, __m128i z, __m128i w) { x = Add(x, y, z, w); return x; }
__m128i inline Xor(__m128i x, __m128i y) { return _mm_xor_si128(x, y); }
__m128i inline Xor(__m128i x, __m128i y, __m128i z) { return Xor(Xor(x, y), z); }
__m128i inline Or(__m128i x, __m128i y) { return _mm_or_si128(x, y); }
__m128i inline And(__m128i x, __m128i y) { return _mm_and_si128(x, y); }
__m128i inline ShR(__m128i x, int n) { return _mm_srli_epi32(x, n); }
__m128i inline ShL(__m128i x, int n) { return _mm_slli_epi32(x, n); }

__m128i inline Ch(__m128i x, __m128i y, __m128i z) { return Xor(z, And(x, Xor(y, z))); }
