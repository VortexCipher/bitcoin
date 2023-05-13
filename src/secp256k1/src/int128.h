#ifndef SECP256K1_INT128_H
#define SECP256K1_INT128_H

#include "util.h"

#if defined(SECP256K1_WIDEMUL_INT128)
#  if defined(SECP256K1_INT128_NATIVE)
#    include "int128_native.h"
#  elif defined(SECP256K1_INT128_STRUCT)
