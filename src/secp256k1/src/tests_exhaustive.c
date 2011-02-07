/***********************************************************************
 * Copyright (c) 2016 Andrew Poelstra                                  *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef EXHAUSTIVE_TEST_ORDER
/* see group_impl.h for allowable values */
#define EXHAUSTIVE_TEST_ORDER 13
#endif

/* These values of B are all values in [1, 8] that result in a curve with even order. */
#define EXHAUSTIVE_TEST_CURVE_HAS_EVEN_ORDER (SECP256K1_B == 1 || SECP256K1_B == 6 || SECP256K1_B == 8)

#ifdef USE_EXTERNAL_DEFAULT_CALLBACKS
    #pragma message("Ignoring USE_EXTERNAL_CALLBACKS in exhaustive_tests.")
    #undef USE_EXTERNAL_DEFAULT_CALLBACKS
#endif
#include "secp256k1.c"

#include "../include/secp256k1.h"
#include "assumptions.h"
#include "group.h"
#include "testrand_impl.h"
#include "ecmult_compute_table_impl.h"
#include "ecmult_gen_compute_table_impl.h"
#include "testutil.h"
#include "util.h"

static int count = 2;

static uint32_t num_cores = 1;
static uint32_t this_core = 0;

SECP256K1_INLINE static int skip_section(uint64_t* iter) {
    if (num_cores == 1) return 0;
    *iter += 0xe7037ed1a0b428dbULL;
    return ((((uint32_t)*iter ^ (*iter >> 32)) * num_cores) >> 32) != this_core;
}

static int secp256k1_nonce_function_smallint(unsigned char *nonce32, const unsigned char *msg32,
                                      const unsigned char *key32, const unsigned char *algo16,
