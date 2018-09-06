/***********************************************************************
 * Copyright (c) 2018-2020 Andrew Poelstra, Jonas Nick                 *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef SECP256K1_MODULE_SCHNORRSIG_TESTS_H
#define SECP256K1_MODULE_SCHNORRSIG_TESTS_H

#include "../../../include/secp256k1_schnorrsig.h"

/* Checks that a bit flip in the n_flip-th argument (that has n_bytes many
 * bytes) changes the hash function
 */
static void nonce_function_bip340_bitflip(unsigned char **args, size_t n_flip, size_t n_bytes, size_t msglen, size_t algolen) {
    unsigned char nonces[2][32];
    CHECK(nonce_function_bip340(nonces[0], args[0], msglen, args[1], args[2], args[3], algolen, args[4]) == 1);
    testrand_flip(args[n_flip], n_bytes);
    CHECK(nonce_function_bip340(nonces[1], args[0], msglen, args[1], args[2], args[3], algolen, args[4]) == 1);
    CHECK(secp256k1_memcmp_var(nonces[0], nonces[1], 32) != 0);
}

static void run_nonce_function_bip340_tests(void) {
    unsigned char tag[13] = "BIP0340/nonce";
    unsigned char aux_tag[11] = "BIP0340/aux";
    unsigned char algo[13] = "BIP0340/nonce";
    size_t algolen = sizeof(algo);
    secp256k1_sha256 sha;
    secp256k1_sha256 sha_optimized;
    unsigned char nonce[32], nonce_z[32];
    unsigned char msg[32];
    size_t msglen = sizeof(msg);
    unsigned char key[32];
    unsigned char pk[32];
    unsigned char aux_rand[32];
    unsigned char *args[5];
    int i;

    /* Check that hash initialized by
     * secp256k1_nonce_function_bip340_sha256_tagged has the expected
     * state. */
    secp256k1_sha256_initialize_tagged(&sha, tag, sizeof(tag));
    secp256k1_nonce_function_bip340_sha256_tagged(&sha_optimized);
    test_sha256_eq(&sha, &sha_optimized);

   /* Check that hash initialized by
    * secp256k1_nonce_function_bip340_sha256_tagged_aux has the expected
    * state. */
    secp256k1_sha256_initialize_tagged(&sha, aux_tag, sizeof(aux_tag));
    secp256k1_nonce_function_bip340_sha256_tagged_aux(&sha_optimized);
    test_sha256_eq(&sha, &sha_optimized);

    testrand256(msg);
    testrand256(key);
    testrand256(pk);
    testrand256(aux_rand);

    /* Check that a bitflip in an argument results in different nonces. */
    args[0] = msg;
    args[1] = key;
    args[2] = pk;
    args[3] = algo;
    args[4] = aux_rand;
    for (i = 0; i < COUNT; i++) {
        nonce_function_bip340_bitflip(args, 0, 32, msglen, algolen);
        nonce_function_bip340_bitflip(args, 1, 32, msglen, algolen);
        nonce_function_bip340_bitflip(args, 2, 32, msglen, algolen);
        /* Flip algo special case "BIP0340/nonce" */
        nonce_function_bip340_bitflip(args, 3, algolen, msglen, algolen);
        /* Flip algo again */
        nonce_function_bip340_bitflip(args, 3, algolen, msglen, algolen);
        nonce_function_bip340_bitflip(args, 4, 32, msglen, algolen);
    }

    /* NULL algo is disallowed */
    CHECK(nonce_function_bip340(nonce, msg, msglen, key, pk, NULL, 0, NULL) == 0);
    CHECK(nonce_function_bip340(nonce, msg, msglen, key, pk, algo, algolen, NULL) == 1);
    /* Other algo is fine */
    testrand_bytes_test(algo, algolen);
    CHECK(nonce_function_bip340(nonce, msg, msglen, key, pk, algo, algolen, NULL) == 1);

    for (i = 0; i < COUNT; i++) {
        unsigned char nonce2[32];
        uint32_t offset = testrand_int(msglen - 1);
        size_t msglen_tmp = (msglen + offset) % msglen;
        size_t algolen_tmp;

        /* Different msglen gives different nonce */
        CHECK(nonce_function_bip340(nonce2, msg, msglen_tmp, key, pk, algo, algolen, NULL) == 1);
        CHECK(secp256k1_memcmp_var(nonce, nonce2, 32) != 0);

        /* Different algolen gives different nonce */
        offset = testrand_int(algolen - 1);
        algolen_tmp = (algolen + offset) % algolen;
        CHECK(nonce_function_bip340(nonce2, msg, msglen, key, pk, algo, algolen_tmp, NULL) == 1);
        CHECK(secp256k1_memcmp_var(nonce, nonce2, 32) != 0);
    }

