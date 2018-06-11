/***********************************************************************
 * Copyright (c) 2014 Pieter Wuille                                    *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef SECP256K1_BENCH_H
#define SECP256K1_BENCH_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if (defined(_MSC_VER) && _MSC_VER >= 1900)
#  include <time.h>
#else
#  include <sys/time.h>
