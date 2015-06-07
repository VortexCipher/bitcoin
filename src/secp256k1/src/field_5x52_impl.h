/***********************************************************************
 * Copyright (c) 2013, 2014 Pieter Wuille                              *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef SECP256K1_FIELD_REPR_IMPL_H
#define SECP256K1_FIELD_REPR_IMPL_H

#include "checkmem.h"
#include "util.h"
#include "field.h"
#include "modinv64_impl.h"

#include "field_5x52_int128_impl.h"

#ifdef VERIFY
static void secp256k1_fe_impl_verify(const secp256k1_fe *a) {
    const uint64_t *d = a->n;
    int m = a->normalized ? 1 : 2 * a->magnitude;
   /* secp256k1 'p' value defined in "Standards for Efficient Cryptography" (SEC2) 2.7.1. */
    VERIFY_CHECK(d[0] <= 0xFFFFFFFFFFFFFULL * m);
    VERIFY_CHECK(d[1] <= 0xFFFFFFFFFFFFFULL * m);
    VERIFY_CHECK(d[2] <= 0xFFFFFFFFFFFFFULL * m);
    VERIFY_CHECK(d[3] <= 0xFFFFFFFFFFFFFULL * m);
    VERIFY_CHECK(d[4] <= 0x0FFFFFFFFFFFFULL * m);
    if (a->normalized) {
        if ((d[4] == 0x0FFFFFFFFFFFFULL) && ((d[3] & d[2] & d[1]) == 0xFFFFFFFFFFFFFULL)) {
            VERIFY_CHECK(d[0] < 0xFFFFEFFFFFC2FULL);
        }
    }
}
#endif

static void secp256k1_fe_impl_get_bounds(secp256k1_fe *r, int m) {
    r->n[0] = 0xFFFFFFFFFFFFFULL * 2 * m;
    r->n[1] = 0xFFFFFFFFFFFFFULL * 2 * m;
    r->n[2] = 0xFFFFFFFFFFFFFULL * 2 * m;
    r->n[3] = 0xFFFFFFFFFFFFFULL * 2 * m;
    r->n[4] = 0x0FFFFFFFFFFFFULL * 2 * m;
}

static void secp256k1_fe_impl_normalize(secp256k1_fe *r) {
    uint64_t t0 = r->n[0], t1 = r->n[1], t2 = r->n[2], t3 = r->n[3], t4 = r->n[4];

    /* Reduce t4 at the start so there will be at most a single carry from the first pass */
    uint64_t m;
    uint64_t x = t4 >> 48; t4 &= 0x0FFFFFFFFFFFFULL;

    /* The first pass ensures the magnitude is 1, ... */
    t0 += x * 0x1000003D1ULL;
    t1 += (t0 >> 52); t0 &= 0xFFFFFFFFFFFFFULL;
