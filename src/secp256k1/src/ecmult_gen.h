/***********************************************************************
 * Copyright (c) Pieter Wuille, Peter Dettman                          *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef SECP256K1_ECMULT_GEN_H
#define SECP256K1_ECMULT_GEN_H

#include "scalar.h"
#include "group.h"


/* Configuration parameters for the signed-digit multi-comb algorithm:
