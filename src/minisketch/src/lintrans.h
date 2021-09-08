/**********************************************************************
 * Copyright (c) 2018 Pieter Wuille, Greg Maxwell, Gleb Naumenko      *
 * Distributed under the MIT software license, see the accompanying   *
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.*
 **********************************************************************/

#ifndef _MINISKETCH_LINTRANS_H_
#define _MINISKETCH_LINTRANS_H_

#include "int_utils.h"

/** A type to represent integers in the type system. */
template<int N> struct Num {};

/** A Linear N-bit transformation over the field I. */
