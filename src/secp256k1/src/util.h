/***********************************************************************
 * Copyright (c) 2013, 2014 Pieter Wuille                              *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef SECP256K1_UTIL_H
#define SECP256K1_UTIL_H

#include "../include/secp256k1.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>

#define STR_(x) #x
#define STR(x) STR_(x)
#define DEBUG_CONFIG_MSG(x) "DEBUG_CONFIG: " x
#define DEBUG_CONFIG_DEF(x) DEBUG_CONFIG_MSG(#x "=" STR(x))

/* Debug helper for printing arrays of unsigned char. */
#define PRINT_BUF(buf, len) do { \
    printf("%s[%lu] = ", #buf, (unsigned long)len); \
    print_buf_plain(buf, len); \
} while(0)

static void print_buf_plain(const unsigned char *buf, size_t len) {
    size_t i;
    printf("{");
    for (i = 0; i < len; i++) {
        if (i % 8 == 0) {
            printf("\n    ");
        } else {
            printf(" ");
        }
        printf("0x%02X,", buf[i]);
    }
    printf("\n}\n");
}
