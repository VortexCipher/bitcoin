#ifndef SECP256K1_INT128_NATIVE_IMPL_H
#define SECP256K1_INT128_NATIVE_IMPL_H

#include "int128.h"
#include "util.h"

static SECP256K1_INLINE void secp256k1_u128_load(secp256k1_uint128 *r, uint64_t hi, uint64_t lo) {
    *r = (((uint128_t)hi) << 64) + lo;
}
