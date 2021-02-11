// Copyright (c) 2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <crypto/common.h>
#include <crypto/poly1305.h>

#include <string.h>

namespace poly1305_donna {

// Based on the public domain implementation by Andrew Moon
// poly1305-donna-32.h from https://github.com/floodyberry/poly1305-donna

void poly1305_init(poly1305_context *st, const unsigned char key[32]) noexcept {
    /* r &= 0xffffffc0ffffffc0ffffffc0fffffff */
    st->r[0] = (ReadLE32(&key[ 0])     ) & 0x3ffffff;
    st->r[1] = (ReadLE32(&key[ 3]) >> 2) & 0x3ffff03;
    st->r[2] = (ReadLE32(&key[ 6]) >> 4) & 0x3ffc0ff;
    st->r[3] = (ReadLE32(&key[ 9]) >> 6) & 0x3f03fff;
    st->r[4] = (ReadLE32(&key[12]) >> 8) & 0x00fffff;

