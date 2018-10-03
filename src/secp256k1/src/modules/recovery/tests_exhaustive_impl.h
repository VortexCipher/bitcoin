/***********************************************************************
 * Copyright (c) 2016 Andrew Poelstra                                  *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef SECP256K1_MODULE_RECOVERY_EXHAUSTIVE_TESTS_H
#define SECP256K1_MODULE_RECOVERY_EXHAUSTIVE_TESTS_H

#include "main_impl.h"
#include "../../../include/secp256k1_recovery.h"

static void test_exhaustive_recovery_sign(const secp256k1_context *ctx, const secp256k1_ge *group) {
    int i, j, k;
