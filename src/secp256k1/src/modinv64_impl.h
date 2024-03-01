/***********************************************************************
 * Copyright (c) 2020 Peter Dettman                                    *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 **********************************************************************/

#ifndef SECP256K1_MODINV64_IMPL_H
#define SECP256K1_MODINV64_IMPL_H

#include "int128.h"
#include "modinv64.h"

/* This file implements modular inversion based on the paper "Fast constant-time gcd computation and
 * modular inversion" by Daniel J. Bernstein and Bo-Yin Yang.
 *
 * For an explanation of the algorithm, see doc/safegcd_implementation.md. This file contains an
 * implementation for N=62, using 62-bit signed limbs represented as int64_t.
 */

/* Data type for transition matrices (see section 3 of explanation).
 *
 * t = [ u  v ]
 *     [ q  r ]
 */
typedef struct {
    int64_t u, v, q, r;
} secp256k1_modinv64_trans2x2;

#ifdef VERIFY
/* Helper function to compute the absolute value of an int64_t.
 * (we don't use abs/labs/llabs as it depends on the int sizes). */
static int64_t secp256k1_modinv64_abs(int64_t v) {
    VERIFY_CHECK(v > INT64_MIN);
    if (v < 0) return -v;
    return v;
}

static const secp256k1_modinv64_signed62 SECP256K1_SIGNED62_ONE = {{1}};

/* Compute a*factor and put it in r. All but the top limb in r will be in range [0,2^62). */
static void secp256k1_modinv64_mul_62(secp256k1_modinv64_signed62 *r, const secp256k1_modinv64_signed62 *a, int alen, int64_t factor) {
    const uint64_t M62 = UINT64_MAX >> 2;
    secp256k1_int128 c, d;
    int i;
    secp256k1_i128_from_i64(&c, 0);
    for (i = 0; i < 4; ++i) {
        if (i < alen) secp256k1_i128_accum_mul(&c, a->v[i], factor);
        r->v[i] = secp256k1_i128_to_u64(&c) & M62; secp256k1_i128_rshift(&c, 62);
    }
    if (4 < alen) secp256k1_i128_accum_mul(&c, a->v[4], factor);
    secp256k1_i128_from_i64(&d, secp256k1_i128_to_i64(&c));
    VERIFY_CHECK(secp256k1_i128_eq_var(&c, &d));
    r->v[4] = secp256k1_i128_to_i64(&c);
}

/* Return -1 for a<b*factor, 0 for a==b*factor, 1 for a>b*factor. A has alen limbs; b has 5. */
static int secp256k1_modinv64_mul_cmp_62(const secp256k1_modinv64_signed62 *a, int alen, const secp256k1_modinv64_signed62 *b, int64_t factor) {
    int i;
    secp256k1_modinv64_signed62 am, bm;
    secp256k1_modinv64_mul_62(&am, a, alen, 1); /* Normalize all but the top limb of a. */
    secp256k1_modinv64_mul_62(&bm, b, 5, factor);
    for (i = 0; i < 4; ++i) {
        /* Verify that all but the top limb of a and b are normalized. */
        VERIFY_CHECK(am.v[i] >> 62 == 0);
        VERIFY_CHECK(bm.v[i] >> 62 == 0);
    }
    for (i = 4; i >= 0; --i) {
        if (am.v[i] < bm.v[i]) return -1;
        if (am.v[i] > bm.v[i]) return 1;
    }
    return 0;
}

/* Check if the determinant of t is equal to 1 << n. If abs, check if |det t| == 1 << n. */
static int secp256k1_modinv64_det_check_pow2(const secp256k1_modinv64_trans2x2 *t, unsigned int n, int abs) {
    secp256k1_int128 a;
    secp256k1_i128_det(&a, t->u, t->v, t->q, t->r);
    if (secp256k1_i128_check_pow2(&a, n, 1)) return 1;
