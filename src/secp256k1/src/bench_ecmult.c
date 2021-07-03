/***********************************************************************
 * Copyright (c) 2017 Pieter Wuille                                    *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/
#include <stdio.h>

#include "secp256k1.c"
#include "../include/secp256k1.h"

#include "util.h"
#include "hash_impl.h"
#include "field_impl.h"
#include "group_impl.h"
#include "scalar_impl.h"
#include "ecmult_impl.h"
#include "bench.h"

#define POINTS 32768

static void help(char **argv) {
    printf("Benchmark EC multiplication algorithms\n");
    printf("\n");
    printf("Usage: %s <help|pippenger_wnaf|strauss_wnaf|simple>\n", argv[0]);
    printf("The output shows the number of multiplied and summed points right after the\n");
    printf("function name. The letter 'g' indicates that one of the points is the generator.\n");
    printf("The benchmarks are divided by the number of points.\n");
    printf("\n");
    printf("default (ecmult_multi): picks pippenger_wnaf or strauss_wnaf depending on the\n");
    printf("                        batch size\n");
    printf("pippenger_wnaf:         for all batch sizes\n");
    printf("strauss_wnaf:           for all batch sizes\n");
    printf("simple:                 multiply and sum each point individually\n");
}

typedef struct {
