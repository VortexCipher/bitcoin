/***********************************************************************
 * Copyright (c) 2013, 2014 Pieter Wuille                              *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef SECP256K1_FIELD_INNER5X52_IMPL_H
#define SECP256K1_FIELD_INNER5X52_IMPL_H

#include <stdint.h>

#include "int128.h"
#include "util.h"

#define VERIFY_BITS(x, n) VERIFY_CHECK(((x) >> (n)) == 0)
#define VERIFY_BITS_128(x, n) VERIFY_CHECK(secp256k1_u128_check_bits((x), (n)))

SECP256K1_INLINE static void secp256k1_fe_mul_inner(uint64_t *r, const uint64_t *a, const uint64_t * SECP256K1_RESTRICT b) {
    secp256k1_uint128 c, d;
    uint64_t t3, t4, tx, u0;
    uint64_t a0 = a[0], a1 = a[1], a2 = a[2], a3 = a[3], a4 = a[4];
    const uint64_t M = 0xFFFFFFFFFFFFFULL, R = 0x1000003D10ULL;

    VERIFY_BITS(a[0], 56);
    VERIFY_BITS(a[1], 56);
    VERIFY_BITS(a[2], 56);
    VERIFY_BITS(a[3], 56);
