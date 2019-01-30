/***********************************************************************
 * Copyright (c) 2020 Peter Dettman                                    *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 **********************************************************************/

#ifndef SECP256K1_MODINV32_IMPL_H
#define SECP256K1_MODINV32_IMPL_H

#include "modinv32.h"

#include "util.h"

#include <stdlib.h>

/* This file implements modular inversion based on the paper "Fast constant-time gcd computation and
 * modular inversion" by Daniel J. Bernstein and Bo-Yin Yang.
 *
 * For an explanation of the algorithm, see doc/safegcd_implementation.md. This file contains an
 * implementation for N=30, using 30-bit signed limbs represented as int32_t.
 */

#ifdef VERIFY
static const secp256k1_modinv32_signed30 SECP256K1_SIGNED30_ONE = {{1}};

/* Compute a*factor and put it in r. All but the top limb in r will be in range [0,2^30). */
static void secp256k1_modinv32_mul_30(secp256k1_modinv32_signed30 *r, const secp256k1_modinv32_signed30 *a, int alen, int32_t factor) {
    const int32_t M30 = (int32_t)(UINT32_MAX >> 2);
    int64_t c = 0;
    int i;
    for (i = 0; i < 8; ++i) {
        if (i < alen) c += (int64_t)a->v[i] * factor;
        r->v[i] = (int32_t)c & M30; c >>= 30;
    }
    if (8 < alen) c += (int64_t)a->v[8] * factor;
    VERIFY_CHECK(c == (int32_t)c);
    r->v[8] = (int32_t)c;
}

/* Return -1 for a<b*factor, 0 for a==b*factor, 1 for a>b*factor. A consists of alen limbs; b has 9. */
static int secp256k1_modinv32_mul_cmp_30(const secp256k1_modinv32_signed30 *a, int alen, const secp256k1_modinv32_signed30 *b, int32_t factor) {
    int i;
    secp256k1_modinv32_signed30 am, bm;
    secp256k1_modinv32_mul_30(&am, a, alen, 1); /* Normalize all but the top limb of a. */
    secp256k1_modinv32_mul_30(&bm, b, 9, factor);
    for (i = 0; i < 8; ++i) {
        /* Verify that all but the top limb of a and b are normalized. */
        VERIFY_CHECK(am.v[i] >> 30 == 0);
        VERIFY_CHECK(bm.v[i] >> 30 == 0);
    }
    for (i = 8; i >= 0; --i) {
        if (am.v[i] < bm.v[i]) return -1;
        if (am.v[i] > bm.v[i]) return 1;
    }
    return 0;
}
#endif

/* Take as input a signed30 number in range (-2*modulus,modulus), and add a multiple of the modulus
 * to it to bring it to range [0,modulus). If sign < 0, the input will also be negated in the
 * process. The input must have limbs in range (-2^30,2^30). The output will have limbs in range
 * [0,2^30). */
static void secp256k1_modinv32_normalize_30(secp256k1_modinv32_signed30 *r, int32_t sign, const secp256k1_modinv32_modinfo *modinfo) {
    const int32_t M30 = (int32_t)(UINT32_MAX >> 2);
    int32_t r0 = r->v[0], r1 = r->v[1], r2 = r->v[2], r3 = r->v[3], r4 = r->v[4],
            r5 = r->v[5], r6 = r->v[6], r7 = r->v[7], r8 = r->v[8];
    volatile int32_t cond_add, cond_negate;

#ifdef VERIFY
    /* Verify that all limbs are in range (-2^30,2^30). */
    int i;
    for (i = 0; i < 9; ++i) {
