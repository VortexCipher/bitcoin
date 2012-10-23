/**********************************************************************
 * Copyright (c) 2018 Pieter Wuille, Greg Maxwell, Gleb Naumenko      *
 * Distributed under the MIT software license, see the accompanying   *
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.*
 **********************************************************************/


#include <new>

#define MINISKETCH_BUILD
#ifdef _MINISKETCH_H_
#  error "minisketch.h cannot be included before minisketch.cpp"
#endif
#include "../include/minisketch.h"

#include "false_positives.h"
#include "fielddefines.h"
#include "sketch.h"

#ifdef HAVE_CLMUL
#  ifdef _MSC_VER
#    include <intrin.h>
#  else
#    include <cpuid.h>
#  endif
#endif

Sketch* ConstructGeneric1Byte(int bits, int implementation);
Sketch* ConstructGeneric2Bytes(int bits, int implementation);
Sketch* ConstructGeneric3Bytes(int bits, int implementation);
Sketch* ConstructGeneric4Bytes(int bits, int implementation);
Sketch* ConstructGeneric5Bytes(int bits, int implementation);
Sketch* ConstructGeneric6Bytes(int bits, int implementation);
Sketch* ConstructGeneric7Bytes(int bits, int implementation);
Sketch* ConstructGeneric8Bytes(int bits, int implementation);

#ifdef HAVE_CLMUL
Sketch* ConstructClMul1Byte(int bits, int implementation);
Sketch* ConstructClMul2Bytes(int bits, int implementation);
Sketch* ConstructClMul3Bytes(int bits, int implementation);
Sketch* ConstructClMul4Bytes(int bits, int implementation);
Sketch* ConstructClMul5Bytes(int bits, int implementation);
Sketch* ConstructClMul6Bytes(int bits, int implementation);
Sketch* ConstructClMul7Bytes(int bits, int implementation);
Sketch* ConstructClMul8Bytes(int bits, int implementation);
Sketch* ConstructClMulTri1Byte(int bits, int implementation);
Sketch* ConstructClMulTri2Bytes(int bits, int implementation);
Sketch* ConstructClMulTri3Bytes(int bits, int implementation);
