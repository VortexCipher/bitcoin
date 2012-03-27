#ifndef SECP256K1_PREALLOCATED_H
#define SECP256K1_PREALLOCATED_H

#include "secp256k1.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The module provided by this header file is intended for settings in which it
 * is not possible or desirable to rely on dynamic memory allocation. It provides
 * functions for creating, cloning, and destroying secp256k1 context objects in a
 * contiguous fixed-size block of memory provided by the caller.
