// Copyright (c) 2017-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifdef ENABLE_AVX2

#include <stdint.h>
#include <immintrin.h>

#include <attributes.h>
#include <crypto/common.h>

namespace sha256d64_avx2 {
namespace {

__m256i inline K(uint32_t x) { return _mm256_set1_epi32(x); }

__m256i inline Add(__m256i x, __m256i y) { return _mm256_add_epi32(x, y); }
__m256i inline Add(__m256i x, __m256i y, __m256i z) { return Add(Add(x, y), z); }
__m256i inline Add(__m256i x, __m256i y, __m256i z, __m256i w) { return Add(Add(x, y), Add(z, w)); }
__m256i inline Add(__m256i x, __m256i y, __m256i z, __m256i w, __m256i v) { return Add(Add(x, y, z), Add(w, v)); }
__m256i inline Inc(__m256i& x, __m256i y) { x = Add(x, y); return x; }
__m256i inline Inc(__m256i& x, __m256i y, __m256i z) { x = Add(x, y, z); return x; }
__m256i inline Inc(__m256i& x, __m256i y, __m256i z, __m256i w) { x = Add(x, y, z, w); return x; }
__m256i inline Xor(__m256i x, __m256i y) { return _mm256_xor_si256(x, y); }
__m256i inline Xor(__m256i x, __m256i y, __m256i z) { return Xor(Xor(x, y), z); }
__m256i inline Or(__m256i x, __m256i y) { return _mm256_or_si256(x, y); }
__m256i inline And(__m256i x, __m256i y) { return _mm256_and_si256(x, y); }
__m256i inline ShR(__m256i x, int n) { return _mm256_srli_epi32(x, n); }
__m256i inline ShL(__m256i x, int n) { return _mm256_slli_epi32(x, n); }

__m256i inline Ch(__m256i x, __m256i y, __m256i z) { return Xor(z, And(x, Xor(y, z))); }
