/***********************************************************************
 * Copyright (c) 2020 Jonas Nick                                       *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef SECP256K1_MODULE_EXTRAKEYS_TESTS_H
#define SECP256K1_MODULE_EXTRAKEYS_TESTS_H

#include "../../../include/secp256k1_extrakeys.h"

static void test_xonly_pubkey(void) {
    secp256k1_pubkey pk;
    secp256k1_xonly_pubkey xonly_pk, xonly_pk_tmp;
    secp256k1_ge pk1;
    secp256k1_ge pk2;
    secp256k1_fe y;
    unsigned char sk[32];
    unsigned char xy_sk[32];
    unsigned char buf32[32];
    unsigned char ones32[32];
    unsigned char zeros64[64] = { 0 };
    int pk_parity;
    int i;

    testrand256(sk);
    memset(ones32, 0xFF, 32);
    testrand256(xy_sk);
    CHECK(secp256k1_ec_pubkey_create(CTX, &pk, sk) == 1);
    CHECK(secp256k1_xonly_pubkey_from_pubkey(CTX, &xonly_pk, &pk_parity, &pk) == 1);

    /* Test xonly_pubkey_from_pubkey */
    CHECK(secp256k1_xonly_pubkey_from_pubkey(CTX, &xonly_pk, &pk_parity, &pk) == 1);
    CHECK_ILLEGAL(CTX, secp256k1_xonly_pubkey_from_pubkey(CTX, NULL, &pk_parity, &pk));
    CHECK(secp256k1_xonly_pubkey_from_pubkey(CTX, &xonly_pk, NULL, &pk) == 1);
    CHECK_ILLEGAL(CTX, secp256k1_xonly_pubkey_from_pubkey(CTX, &xonly_pk, &pk_parity, NULL));
    memset(&pk, 0, sizeof(pk));
    CHECK_ILLEGAL(CTX, secp256k1_xonly_pubkey_from_pubkey(CTX, &xonly_pk, &pk_parity, &pk));

    /* Choose a secret key such that the resulting pubkey and xonly_pubkey match. */
    memset(sk, 0, sizeof(sk));
    sk[0] = 1;
    CHECK(secp256k1_ec_pubkey_create(CTX, &pk, sk) == 1);
    CHECK(secp256k1_xonly_pubkey_from_pubkey(CTX, &xonly_pk, &pk_parity, &pk) == 1);
    CHECK(secp256k1_memcmp_var(&pk, &xonly_pk, sizeof(pk)) == 0);
    CHECK(pk_parity == 0);

    /* Choose a secret key such that pubkey and xonly_pubkey are each others
