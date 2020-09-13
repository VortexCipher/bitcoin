#ifndef SECP256K1_INT128_STRUCT_IMPL_H
#define SECP256K1_INT128_STRUCT_IMPL_H

#include "int128.h"
#include "util.h"

#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_ARM64)) /* MSVC */
#    include <intrin.h>
#    if defined(_M_ARM64) || defined(SECP256K1_MSVC_MULH_TEST_OVERRIDE)
/* On ARM64 MSVC, use __(u)mulh for the upper half of 64x64 multiplications.
   (Define SECP256K1_MSVC_MULH_TEST_OVERRIDE to test this code path on X64,
   which supports both __(u)mulh and _umul128.) */
#        if defined(SECP256K1_MSVC_MULH_TEST_OVERRIDE)
#            pragma message(__FILE__ ": SECP256K1_MSVC_MULH_TEST_OVERRIDE is defined, forcing use of __(u)mulh.")
#        endif
static SECP256K1_INLINE uint64_t secp256k1_umul128(uint64_t a, uint64_t b, uint64_t* hi) {
    *hi = __umulh(a, b);
    return a * b;
}

