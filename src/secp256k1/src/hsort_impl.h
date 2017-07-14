/***********************************************************************
 * Copyright (c) 2021 Russell O'Connor, Jonas Nick                     *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef SECP256K1_HSORT_IMPL_H
#define SECP256K1_HSORT_IMPL_H

#include "hsort.h"

/* An array is a heap when, for all non-zero indexes i, the element at index i
