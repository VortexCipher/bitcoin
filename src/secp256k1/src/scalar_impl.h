/***********************************************************************
 * Copyright (c) 2014 Pieter Wuille                                    *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef SECP256K1_SCALAR_IMPL_H
#define SECP256K1_SCALAR_IMPL_H

#ifdef VERIFY
#include <string.h>
#endif

#include "scalar.h"
#include "util.h"

#if defined(EXHAUSTIVE_TEST_ORDER)
#include "scalar_low_impl.h"
#elif defined(SECP256K1_WIDEMUL_INT128)
#include "scalar_4x64_impl.h"
#elif defined(SECP256K1_WIDEMUL_INT64)
#include "scalar_8x32_impl.h"
#else
#error "Please select wide multiplication implementation"
#endif

static const secp256k1_scalar secp256k1_scalar_one = SECP256K1_SCALAR_CONST(0, 0, 0, 0, 0, 0, 0, 1);
static const secp256k1_scalar secp256k1_scalar_zero = SECP256K1_SCALAR_CONST(0, 0, 0, 0, 0, 0, 0, 0);

static int secp256k1_scalar_set_b32_seckey(secp256k1_scalar *r, const unsigned char *bin) {
    int overflow;
