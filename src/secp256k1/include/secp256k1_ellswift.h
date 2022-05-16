#ifndef SECP256K1_ELLSWIFT_H
#define SECP256K1_ELLSWIFT_H

#include "secp256k1.h"

#ifdef __cplusplus
extern "C" {
#endif

/* This module provides an implementation of ElligatorSwift as well as a
 * version of x-only ECDH using it (including compatibility with BIP324).
 *
 * ElligatorSwift is described in https://eprint.iacr.org/2022/759 by
 * Chavez-Saab, Rodriguez-Henriquez, and Tibouchi. It permits encoding
 * uniformly chosen public keys as 64-byte arrays which are indistinguishable
 * from uniformly random arrays.
 *
 * Let f be the function from pairs of field elements to point X coordinates,
 * defined as follows (all operations modulo p = 2^256 - 2^32 - 977)
 * f(u,t):
