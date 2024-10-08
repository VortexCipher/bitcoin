/***********************************************************************
 * Copyright (c) 2013, 2014 Pieter Wuille                              *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef SECP256K1_GROUP_IMPL_H
#define SECP256K1_GROUP_IMPL_H

#include "field.h"
#include "group.h"
#include "util.h"

/* Begin of section generated by sage/gen_exhaustive_groups.sage. */
#define SECP256K1_G_ORDER_7 SECP256K1_GE_CONST(\
    0x66625d13, 0x317ffe44, 0x63d32cff, 0x1ca02b9b,\
    0xe5c6d070, 0x50b4b05e, 0x81cc30db, 0xf5166f0a,\
    0x1e60e897, 0xa7c00c7c, 0x2df53eb6, 0x98274ff4,\
    0x64252f42, 0x8ca44e17, 0x3b25418c, 0xff4ab0cf\
)
#define SECP256K1_G_ORDER_13 SECP256K1_GE_CONST(\
    0xa2482ff8, 0x4bf34edf, 0xa51262fd, 0xe57921db,\
    0xe0dd2cb7, 0xa5914790, 0xbc71631f, 0xc09704fb,\
    0x942536cb, 0xa3e49492, 0x3a701cc3, 0xee3e443f,\
    0xdf182aa9, 0x15b8aa6a, 0x166d3b19, 0xba84b045\
)
#define SECP256K1_G_ORDER_199 SECP256K1_GE_CONST(\
    0x7fb07b5c, 0xd07c3bda, 0x553902e2, 0x7a87ea2c,\
    0x35108a7f, 0x051f41e5, 0xb76abad5, 0x1f2703ad,\
    0x0a251539, 0x5b4c4438, 0x952a634f, 0xac10dd4d,\
    0x6d6f4745, 0x98990c27, 0x3a4f3116, 0xd32ff969\
)
/** Generator for secp256k1, value 'g' defined in
 *  "Standards for Efficient Cryptography" (SEC2) 2.7.1.
 */
#define SECP256K1_G SECP256K1_GE_CONST(\
    0x79be667e, 0xf9dcbbac, 0x55a06295, 0xce870b07,\
    0x029bfcdb, 0x2dce28d9, 0x59f2815b, 0x16f81798,\
    0x483ada77, 0x26a3c465, 0x5da4fbfc, 0x0e1108a8,\
    0xfd17b448, 0xa6855419, 0x9c47d08f, 0xfb10d4b8\
)
/* These exhaustive group test orders and generators are chosen such that:
 * - The field size is equal to that of secp256k1, so field code is the same.
 * - The curve equation is of the form y^2=x^3+B for some small constant B.
 * - The subgroup has a generator 2*P, where P.x is as small as possible.
 * - The subgroup has size less than 1000 to permit exhaustive testing.
 * - The subgroup admits an endomorphism of the form lambda*(x,y) == (beta*x,y).
 */
#if defined(EXHAUSTIVE_TEST_ORDER)
#  if EXHAUSTIVE_TEST_ORDER == 7

static const secp256k1_ge secp256k1_ge_const_g = SECP256K1_G_ORDER_7;
#define SECP256K1_B 6

#  elif EXHAUSTIVE_TEST_ORDER == 13

static const secp256k1_ge secp256k1_ge_const_g = SECP256K1_G_ORDER_13;
#define SECP256K1_B 2

#  elif EXHAUSTIVE_TEST_ORDER == 199

static const secp256k1_ge secp256k1_ge_const_g = SECP256K1_G_ORDER_199;
#define SECP256K1_B 4

#  else
#    error No known generator for the specified exhaustive test group order.
#  endif
#else

static const secp256k1_ge secp256k1_ge_const_g = SECP256K1_G;
#define SECP256K1_B 7

#endif
/* End of section generated by sage/gen_exhaustive_groups.sage. */

static void secp256k1_ge_verify(const secp256k1_ge *a) {
    SECP256K1_FE_VERIFY(&a->x);
    SECP256K1_FE_VERIFY(&a->y);
    SECP256K1_FE_VERIFY_MAGNITUDE(&a->x, SECP256K1_GE_X_MAGNITUDE_MAX);
    SECP256K1_FE_VERIFY_MAGNITUDE(&a->y, SECP256K1_GE_Y_MAGNITUDE_MAX);
    VERIFY_CHECK(a->infinity == 0 || a->infinity == 1);
    (void)a;
}

static void secp256k1_gej_verify(const secp256k1_gej *a) {
    SECP256K1_FE_VERIFY(&a->x);
    SECP256K1_FE_VERIFY(&a->y);
    SECP256K1_FE_VERIFY(&a->z);
    SECP256K1_FE_VERIFY_MAGNITUDE(&a->x, SECP256K1_GEJ_X_MAGNITUDE_MAX);
    SECP256K1_FE_VERIFY_MAGNITUDE(&a->y, SECP256K1_GEJ_Y_MAGNITUDE_MAX);
    SECP256K1_FE_VERIFY_MAGNITUDE(&a->z, SECP256K1_GEJ_Z_MAGNITUDE_MAX);
    VERIFY_CHECK(a->infinity == 0 || a->infinity == 1);
    (void)a;
}
