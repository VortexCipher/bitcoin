/***********************************************************************
 * Copyright (c) 2022 Pieter Wuille                                    *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

/* The code here is inspired by Kris Kwiatkowski's approach in
 * https://github.com/kriskwiatkowski/pqc/blob/main/src/common/ct_check.h
 * to provide a general interface for memory-checking mechanisms, primarily
 * for constant-time checking.
