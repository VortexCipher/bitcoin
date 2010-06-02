/***********************************************************************
 * Copyright (c) 2013, 2014 Pieter Wuille                              *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef SECP256K1_GROUP_H
#define SECP256K1_GROUP_H

#include "field.h"

/** A group element in affine coordinates on the secp256k1 curve,
 *  or occasionally on an isomorphic curve of the form y^2 = x^3 + 7*t^6.
 *  Note: For exhaustive test mode, secp256k1 is replaced by a small subgroup of a different curve.
 */
typedef struct {
    secp256k1_fe x;
    secp256k1_fe y;
    int infinity; /* whether this represents the point at infinity */
