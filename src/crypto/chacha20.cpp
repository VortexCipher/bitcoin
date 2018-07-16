// Copyright (c) 2017-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

// Based on the public domain implementation 'merged' by D. J. Bernstein
// See https://cr.yp.to/chacha.html.

#include <crypto/common.h>
#include <crypto/chacha20.h>
#include <support/cleanse.h>
#include <span.h>

#include <algorithm>
#include <bit>
#include <string.h>

#define QUARTERROUND(a,b,c,d) \
  a += b; d = std::rotl(d ^ a, 16); \
  c += d; b = std::rotl(b ^ c, 12); \
  a += b; d = std::rotl(d ^ a, 8); \
  c += d; b = std::rotl(b ^ c, 7);

#define REPEAT10(a) do { {a}; {a}; {a}; {a}; {a}; {a}; {a}; {a}; {a}; {a}; } while(0)

void ChaCha20Aligned::SetKey(Span<const std::byte> key) noexcept
{
    assert(key.size() == KEYLEN);
    input[0] = ReadLE32(UCharCast(key.data() + 0));
    input[1] = ReadLE32(UCharCast(key.data() + 4));
    input[2] = ReadLE32(UCharCast(key.data() + 8));
    input[3] = ReadLE32(UCharCast(key.data() + 12));
    input[4] = ReadLE32(UCharCast(key.data() + 16));
    input[5] = ReadLE32(UCharCast(key.data() + 20));
    input[6] = ReadLE32(UCharCast(key.data() + 24));
    input[7] = ReadLE32(UCharCast(key.data() + 28));
    input[8] = 0;
    input[9] = 0;
