// Copyright (c) 2017-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
//
// This is a translation to GCC extended asm syntax from YASM code by Intel
// (available at the bottom of this file).

#include <cstdlib>
#include <stdint.h>

#if defined(__x86_64__) || defined(__amd64__)

namespace sha256_sse4
{
void Transform(uint32_t* s, const unsigned char* chunk, size_t blocks)
#if defined(__clang__) && !defined(__OPTIMIZE__)
  /*
  clang is unable to compile this with -O0 and -fsanitize=address.
  See upstream bug: https://github.com/llvm/llvm-project/issues/92182
  */
  __attribute__((no_sanitize("address")))
#endif
{
    static const uint32_t K256 alignas(16) [] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
    };
    static const uint32_t FLIP_MASK alignas(16) [] = {0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f};
    static const uint32_t SHUF_00BA alignas(16) [] = {0x03020100, 0x0b0a0908, 0xffffffff, 0xffffffff};
    static const uint32_t SHUF_DC00 alignas(16) [] = {0xffffffff, 0xffffffff, 0x03020100, 0x0b0a0908};
    uint32_t a, b, c, d, f, g, h, y0, y1, y2;
    uint64_t tbl;
    uint64_t inp_end, inp;
    uint32_t xfer alignas(16) [4];

    __asm__ __volatile__(
        "shl    $0x6,%2;"
        "je     Ldone_hash_%=;"
        "add    %1,%2;"
        "mov    %2,%14;"
        "mov    (%0),%3;"
        "mov    0x4(%0),%4;"
        "mov    0x8(%0),%5;"
        "mov    0xc(%0),%6;"
        "mov    0x10(%0),%k2;"
        "mov    0x14(%0),%7;"
        "mov    0x18(%0),%8;"
        "mov    0x1c(%0),%9;"
        "movdqa %18,%%xmm12;"
        "movdqa %19,%%xmm10;"
        "movdqa %20,%%xmm11;"

        "Lloop0_%=:"
        "lea    %17,%13;"
        "movdqu (%1),%%xmm4;"
        "pshufb %%xmm12,%%xmm4;"
        "movdqu 0x10(%1),%%xmm5;"
        "pshufb %%xmm12,%%xmm5;"
        "movdqu 0x20(%1),%%xmm6;"
        "pshufb %%xmm12,%%xmm6;"
        "movdqu 0x30(%1),%%xmm7;"
        "pshufb %%xmm12,%%xmm7;"
        "mov    %1,%15;"
        "mov    $3,%1;"

        "Lloop1_%=:"
        "movdqa 0x0(%13),%%xmm9;"
        "paddd  %%xmm4,%%xmm9;"
        "movdqa %%xmm9,%16;"
        "movdqa %%xmm7,%%xmm0;"
        "mov    %k2,%10;"
        "ror    $0xe,%10;"
        "mov    %3,%11;"
        "palignr $0x4,%%xmm6,%%xmm0;"
        "ror    $0x9,%11;"
        "xor    %k2,%10;"
        "mov    %7,%12;"
        "ror    $0x5,%10;"
        "movdqa %%xmm5,%%xmm1;"
        "xor    %3,%11;"
        "xor    %8,%12;"
        "paddd  %%xmm4,%%xmm0;"
        "xor    %k2,%10;"
        "and    %k2,%12;"
        "ror    $0xb,%11;"
        "palignr $0x4,%%xmm4,%%xmm1;"
        "xor    %3,%11;"
        "ror    $0x6,%10;"
        "xor    %8,%12;"
        "movdqa %%xmm1,%%xmm2;"
        "ror    $0x2,%11;"
        "add    %10,%12;"
        "add    %16,%12;"
        "movdqa %%xmm1,%%xmm3;"
        "mov    %3,%10;"
        "add    %12,%9;"
        "mov    %3,%12;"
        "pslld  $0x19,%%xmm1;"
        "or     %5,%10;"
        "add    %9,%6;"
        "and    %5,%12;"
        "psrld  $0x7,%%xmm2;"
        "and    %4,%10;"
        "add    %11,%9;"
        "por    %%xmm2,%%xmm1;"
        "or     %12,%10;"
        "add    %10,%9;"
        "movdqa %%xmm3,%%xmm2;"
        "mov    %6,%10;"
        "mov    %9,%11;"
        "movdqa %%xmm3,%%xmm8;"
        "ror    $0xe,%10;"
        "xor    %6,%10;"
        "mov    %k2,%12;"
        "ror    $0x9,%11;"
        "pslld  $0xe,%%xmm3;"
        "xor    %9,%11;"
        "ror    $0x5,%10;"
        "xor    %7,%12;"
        "psrld  $0x12,%%xmm2;"
        "ror    $0xb,%11;"
        "xor    %6,%10;"
        "and    %6,%12;"
        "ror    $0x6,%10;"
        "pxor   %%xmm3,%%xmm1;"
        "xor    %9,%11;"
        "xor    %7,%12;"
        "psrld  $0x3,%%xmm8;"
        "add    %10,%12;"
        "add    4+%16,%12;"
        "ror    $0x2,%11;"
        "pxor   %%xmm2,%%xmm1;"
        "mov    %9,%10;"
        "add    %12,%8;"
        "mov    %9,%12;"
        "pxor   %%xmm8,%%xmm1;"
        "or     %4,%10;"
