/***********************************************************************
 * Copyright (c) 2014-2015 Pieter Wuille                               *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/
#include <stdio.h>

#include "secp256k1.c"
#include "../include/secp256k1.h"

#include "assumptions.h"
#include "util.h"
#include "hash_impl.h"
#include "field_impl.h"
#include "group_impl.h"
#include "scalar_impl.h"
#include "ecmult_impl.h"
#include "bench.h"

static void help(int default_iters) {
    printf("Benchmarks various internal routines.\n");
    printf("\n");
    printf("The default number of iterations for each benchmark is %d. This can be\n", default_iters);
    printf("customized using the SECP256K1_BENCH_ITERS environment variable.\n");
    printf("\n");
    printf("Usage: ./bench_internal [args]\n");
    printf("By default, all benchmarks will be run.\n");
    printf("args:\n");
    printf("    help       : display this help and exit\n");
    printf("    scalar     : all scalar operations (add, half, inverse, mul, negate, split)\n");
    printf("    field      : all field operations (half, inverse, issquare, mul, normalize, sqr, sqrt)\n");
    printf("    group      : all group operations (add, double, to_affine)\n");
    printf("    ecmult     : all point multiplication operations (ecmult_wnaf) \n");
    printf("    hash       : all hash algorithms (hmac, rng6979, sha256)\n");
    printf("    context    : all context object operations (context_create)\n");
    printf("\n");
}

typedef struct {
    secp256k1_scalar scalar[2];
    secp256k1_fe fe[4];
    secp256k1_ge ge[2];
    secp256k1_gej gej[2];
