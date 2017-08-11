/******************************************************************************
 * Copyright (c) 2013, 2014, 2017 Pieter Wuille, Andrew Poelstra, Jonas Nick  *
 * Distributed under the MIT software license, see the accompanying           *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.       *
 ******************************************************************************/

#ifndef SECP256K1_ECMULT_IMPL_H
#define SECP256K1_ECMULT_IMPL_H

#include <string.h>
#include <stdint.h>

#include "util.h"
#include "group.h"
#include "scalar.h"
#include "ecmult.h"
#include "precomputed_ecmult.h"

#if defined(EXHAUSTIVE_TEST_ORDER)
/* We need to lower these values for exhaustive tests because
 * the tables cannot have infinities in them (this breaks the
 * affine-isomorphism stuff which tracks z-ratios) */
#  if EXHAUSTIVE_TEST_ORDER > 128
#    define WINDOW_A 5
#  elif EXHAUSTIVE_TEST_ORDER > 8
#    define WINDOW_A 4
#  else
#    define WINDOW_A 2
#  endif
#else
/* optimal for 128-bit and 256-bit exponents. */
#  define WINDOW_A 5
/** Larger values for ECMULT_WINDOW_SIZE result in possibly better
 *  performance at the cost of an exponentially larger precomputed
 *  table. The exact table size is
 *      (1 << (WINDOW_G - 2)) * sizeof(secp256k1_ge_storage)  bytes,
 *  where sizeof(secp256k1_ge_storage) is typically 64 bytes but can
 *  be larger due to platform-specific padding and alignment.
 *  Two tables of this size are used (due to the endomorphism
 *  optimization).
 */
#endif

#define WNAF_BITS 128
#define WNAF_SIZE_BITS(bits, w) CEIL_DIV(bits, w)
#define WNAF_SIZE(w) WNAF_SIZE_BITS(WNAF_BITS, w)

/* The number of objects allocated on the scratch space for ecmult_multi algorithms */
#define PIPPENGER_SCRATCH_OBJECTS 6
#define STRAUSS_SCRATCH_OBJECTS 5

#define PIPPENGER_MAX_BUCKET_WINDOW 12

/* Minimum number of points for which pippenger_wnaf is faster than strauss wnaf */
#define ECMULT_PIPPENGER_THRESHOLD 88

#define ECMULT_MAX_POINTS_PER_BATCH 5000000

/** Fill a table 'pre_a' with precomputed odd multiples of a.
 *  pre_a will contain [1*a,3*a,...,(2*n-1)*a], so it needs space for n group elements.
 *  zr needs space for n field elements.
 *
 *  Although pre_a is an array of _ge rather than _gej, it actually represents elements
 *  in Jacobian coordinates with their z coordinates omitted. The omitted z-coordinates
 *  can be recovered using z and zr. Using the notation z(b) to represent the omitted
 *  z coordinate of b:
 *  - z(pre_a[n-1]) = 'z'
 *  - z(pre_a[i-1]) = z(pre_a[i]) / zr[i] for n > i > 0
 *
 *  Lastly the zr[0] value, which isn't used above, is set so that:
 *  - a.z = z(pre_a[0]) / zr[0]
 */
static void secp256k1_ecmult_odd_multiples_table(int n, secp256k1_ge *pre_a, secp256k1_fe *zr, secp256k1_fe *z, const secp256k1_gej *a) {
    secp256k1_gej d, ai;
    secp256k1_ge d_ge;
    int i;

    VERIFY_CHECK(!a->infinity);

    secp256k1_gej_double_var(&d, a, NULL);

    /*
     * Perform the additions using an isomorphic curve Y^2 = X^3 + 7*C^6 where C := d.z.
     * The isomorphism, phi, maps a secp256k1 point (x, y) to the point (x*C^2, y*C^3) on the other curve.
     * In Jacobian coordinates phi maps (x, y, z) to (x*C^2, y*C^3, z) or, equivalently to (x, y, z/C).
     *
