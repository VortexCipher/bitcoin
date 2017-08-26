/***********************************************************************
 * Copyright (c) 2015, 2022 Pieter Wuille, Andrew Poelstra             *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef SECP256K1_ECMULT_CONST_IMPL_H
#define SECP256K1_ECMULT_CONST_IMPL_H

#include "scalar.h"
#include "group.h"
#include "ecmult_const.h"
#include "ecmult_impl.h"

#if defined(EXHAUSTIVE_TEST_ORDER)
/* We need 2^ECMULT_CONST_GROUP_SIZE - 1 to be less than EXHAUSTIVE_TEST_ORDER, because
 * the tables cannot have infinities in them (this breaks the effective-affine technique's
 * z-ratio tracking) */
#  if EXHAUSTIVE_TEST_ORDER == 199
#    define ECMULT_CONST_GROUP_SIZE 4
#  elif EXHAUSTIVE_TEST_ORDER == 13
#    define ECMULT_CONST_GROUP_SIZE 3
#  elif EXHAUSTIVE_TEST_ORDER == 7
#    define ECMULT_CONST_GROUP_SIZE 2
#  else
#    error "Unknown EXHAUSTIVE_TEST_ORDER"
#  endif
#else
/* Group size 4 or 5 appears optimal. */
#  define ECMULT_CONST_GROUP_SIZE 5
#endif

#define ECMULT_CONST_TABLE_SIZE (1L << (ECMULT_CONST_GROUP_SIZE - 1))
#define ECMULT_CONST_GROUPS ((129 + ECMULT_CONST_GROUP_SIZE - 1) / ECMULT_CONST_GROUP_SIZE)
#define ECMULT_CONST_BITS (ECMULT_CONST_GROUPS * ECMULT_CONST_GROUP_SIZE)

/** Fill a table 'pre' with precomputed odd multiples of a.
 *
 *  The resulting point set is brought to a single constant Z denominator, stores the X and Y
