/**********************************************************************
 * Copyright (c) 2018 Pieter Wuille, Greg Maxwell, Gleb Naumenko      *
 * Distributed under the MIT software license, see the accompanying   *
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.*
 **********************************************************************/

#ifndef _MINISKETCH_FIELDS_CLMUL_COMMON_IMPL_H_
#define _MINISKETCH_FIELDS_CLMUL_COMMON_IMPL_H_ 1

#include <stdint.h>
#include <immintrin.h>

#include "../int_utils.h"
#include "../lintrans.h"

namespace {

