/***********************************************************************
 * Copyright (c) 2013, 2014 Pieter Wuille                              *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef SECP256K1_FIELD_H
#define SECP256K1_FIELD_H

#include "util.h"

/* This file defines the generic interface for working with secp256k1_fe
 * objects, which represent field elements (integers modulo 2^256 - 2^32 - 977).
 *
 * The actual definition of the secp256k1_fe type depends on the chosen field
 * implementation; see the field_5x52.h and field_10x26.h files for details.
 *
 * All secp256k1_fe objects have implicit properties that determine what
 * operations are permitted on it. These are purely a function of what
 * secp256k1_fe_ operations are applied on it, generally (implicitly) fixed at
 * compile time, and do not depend on the chosen field implementation. Despite
 * that, what these properties actually entail for the field representation
 * values depends on the chosen field implementation. These properties are:
 * - magnitude: an integer in [0,32]
 * - normalized: 0 or 1; normalized=1 implies magnitude <= 1.
 *
 * In VERIFY mode, they are materialized explicitly as fields in the struct,
 * allowing run-time verification of these properties. In that case, the field
 * implementation also provides a secp256k1_fe_verify routine to verify that
 * these fields match the run-time value and perform internal consistency
 * checks. */
#ifdef VERIFY
#  define SECP256K1_FE_VERIFY_FIELDS \
    int magnitude; \
    int normalized;
