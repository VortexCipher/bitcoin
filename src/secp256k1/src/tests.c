/***********************************************************************
 * Copyright (c) 2013, 2014, 2015 Pieter Wuille, Gregory Maxwell       *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

#ifdef USE_EXTERNAL_DEFAULT_CALLBACKS
    #pragma message("Ignoring USE_EXTERNAL_CALLBACKS in tests.")
    #undef USE_EXTERNAL_DEFAULT_CALLBACKS
#endif
#if defined(VERIFY) && defined(COVERAGE)
    #pragma message("Defining VERIFY for tests being built for coverage analysis support is meaningless.")
#endif
#include "secp256k1.c"

#include "../include/secp256k1.h"
#include "../include/secp256k1_preallocated.h"
#include "testrand_impl.h"
#include "checkmem.h"
#include "testutil.h"
#include "util.h"

#include "../contrib/lax_der_parsing.c"
#include "../contrib/lax_der_privatekey_parsing.c"

#include "modinv32_impl.h"
#ifdef SECP256K1_WIDEMUL_INT128
#include "modinv64_impl.h"
#include "int128_impl.h"
#endif

#define CONDITIONAL_TEST(cnt, nam) if (COUNT < (cnt)) { printf("Skipping %s (iteration count too low)\n", nam); } else

static int COUNT = 64;
static secp256k1_context *CTX = NULL;
static secp256k1_context *STATIC_CTX = NULL;

static int all_bytes_equal(const void* s, unsigned char value, size_t n) {
    const unsigned char *p = s;
    size_t i;

    for (i = 0; i < n; i++) {
        if (p[i] != value) {
            return 0;
        }
    }
    return 1;
}

#define CHECK_COUNTING_CALLBACK_VOID(ctx, expr_or_stmt, callback, callback_setter) do { \
    int32_t _calls_to_callback = 0; \
    secp256k1_callback _saved_callback = ctx->callback; \
    callback_setter(ctx, counting_callback_fn, &_calls_to_callback); \
    { expr_or_stmt; } \
    ctx->callback = _saved_callback; \
    CHECK(_calls_to_callback == 1); \
} while(0);

/* CHECK that expr_or_stmt calls the error or illegal callback of ctx exactly once
 *
 * Useful for checking functions that return void (e.g., API functions that use ARG_CHECK_VOID) */
#define CHECK_ERROR_VOID(ctx, expr_or_stmt) \
    CHECK_COUNTING_CALLBACK_VOID(ctx, expr_or_stmt, error_callback, secp256k1_context_set_error_callback)
#define CHECK_ILLEGAL_VOID(ctx, expr_or_stmt) \
    CHECK_COUNTING_CALLBACK_VOID(ctx, expr_or_stmt, illegal_callback, secp256k1_context_set_illegal_callback)

/* CHECK that
 *  - expr calls the illegal callback of ctx exactly once and,
 *  - expr == 0 (or equivalently, expr == NULL)
 *
 * Useful for checking functions that return an integer or a pointer. */
#define CHECK_ILLEGAL(ctx, expr) CHECK_ILLEGAL_VOID(ctx, CHECK((expr) == 0))
#define CHECK_ERROR(ctx, expr) CHECK_ERROR_VOID(ctx, CHECK((expr) == 0))

static void counting_callback_fn(const char* str, void* data) {
    /* Dummy callback function that just counts. */
    int32_t *p;
    (void)str;
    p = data;
    CHECK(*p != INT32_MAX);
    (*p)++;
}

static void uncounting_illegal_callback_fn(const char* str, void* data) {
    /* Dummy callback function that just counts (backwards). */
    int32_t *p;
    (void)str;
    p = data;
    CHECK(*p != INT32_MIN);
    (*p)--;
}

static void run_xoshiro256pp_tests(void) {
    {
        size_t i;
        /* Sanity check that we run before the actual seeding. */
        for (i = 0; i < sizeof(secp256k1_test_state)/sizeof(secp256k1_test_state[0]); i++) {
            CHECK(secp256k1_test_state[i] == 0);
        }
    }
    {
        int i;
        unsigned char buf32[32];
        unsigned char seed16[16] = {
            'C', 'H', 'I', 'C', 'K', 'E', 'N', '!',
            'C', 'H', 'I', 'C', 'K', 'E', 'N', '!',
        };
        unsigned char buf32_expected[32] = {
            0xAF, 0xCC, 0xA9, 0x16, 0xB5, 0x6C, 0xE3, 0xF0,
            0x44, 0x3F, 0x45, 0xE0, 0x47, 0xA5, 0x08, 0x36,
            0x4C, 0xCC, 0xC1, 0x18, 0xB2, 0xD8, 0x8F, 0xEF,
            0x43, 0x26, 0x15, 0x57, 0x37, 0x00, 0xEF, 0x30,
        };
        testrand_seed(seed16);
        for (i = 0; i < 17; i++) {
            testrand256(buf32);
        }
        CHECK(secp256k1_memcmp_var(buf32, buf32_expected, sizeof(buf32)) == 0);
    }
}

static void run_selftest_tests(void) {
    /* Test public API */
    secp256k1_selftest();
}

static int ecmult_gen_context_eq(const secp256k1_ecmult_gen_context *a, const secp256k1_ecmult_gen_context *b) {
    return a->built == b->built
            && secp256k1_scalar_eq(&a->scalar_offset, &b->scalar_offset)
            && secp256k1_ge_eq_var(&a->ge_offset, &b->ge_offset)
            && secp256k1_fe_equal(&a->proj_blind, &b->proj_blind);
}

static int context_eq(const secp256k1_context *a, const secp256k1_context *b) {
    return a->declassify == b->declassify
            && ecmult_gen_context_eq(&a->ecmult_gen_ctx, &b->ecmult_gen_ctx)
            && a->illegal_callback.fn == b->illegal_callback.fn
            && a->illegal_callback.data == b->illegal_callback.data
            && a->error_callback.fn == b->error_callback.fn
            && a->error_callback.data == b->error_callback.data;
}

static void run_deprecated_context_flags_test(void) {
    /* Check that a context created with any of the flags in the flags array is
     * identical to the NONE context. */
    unsigned int flags[] = { SECP256K1_CONTEXT_SIGN,
                             SECP256K1_CONTEXT_VERIFY,
                             SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY };
    secp256k1_context *none_ctx = secp256k1_context_create(SECP256K1_CONTEXT_NONE);
    int i;
    for (i = 0; i < (int)(sizeof(flags)/sizeof(flags[0])); i++) {
        secp256k1_context *tmp_ctx;
        CHECK(secp256k1_context_preallocated_size(SECP256K1_CONTEXT_NONE) == secp256k1_context_preallocated_size(flags[i]));
        tmp_ctx = secp256k1_context_create(flags[i]);
        CHECK(context_eq(none_ctx, tmp_ctx));
        secp256k1_context_destroy(tmp_ctx);
    }
    secp256k1_context_destroy(none_ctx);
}

static void run_ec_illegal_argument_tests(void) {
    secp256k1_pubkey pubkey;
    secp256k1_pubkey zero_pubkey;
    secp256k1_ecdsa_signature sig;
    unsigned char ctmp[32];

    /* Setup */
    memset(ctmp, 1, 32);
    memset(&zero_pubkey, 0, sizeof(zero_pubkey));

    /* Verify context-type checking illegal-argument errors. */
    CHECK_ILLEGAL(STATIC_CTX, secp256k1_ec_pubkey_create(STATIC_CTX, &pubkey, ctmp));
    SECP256K1_CHECKMEM_UNDEFINE(&pubkey, sizeof(pubkey));
    CHECK(secp256k1_ec_pubkey_create(CTX, &pubkey, ctmp) == 1);
    SECP256K1_CHECKMEM_CHECK(&pubkey, sizeof(pubkey));
    CHECK_ILLEGAL(STATIC_CTX, secp256k1_ecdsa_sign(STATIC_CTX, &sig, ctmp, ctmp, NULL, NULL));
    SECP256K1_CHECKMEM_UNDEFINE(&sig, sizeof(sig));
    CHECK(secp256k1_ecdsa_sign(CTX, &sig, ctmp, ctmp, NULL, NULL) == 1);
    SECP256K1_CHECKMEM_CHECK(&sig, sizeof(sig));
    CHECK(secp256k1_ecdsa_verify(CTX, &sig, ctmp, &pubkey) == 1);
    CHECK(secp256k1_ecdsa_verify(STATIC_CTX, &sig, ctmp, &pubkey) == 1);
    CHECK(secp256k1_ec_pubkey_tweak_add(CTX, &pubkey, ctmp) == 1);
    CHECK(secp256k1_ec_pubkey_tweak_add(STATIC_CTX, &pubkey, ctmp) == 1);
    CHECK(secp256k1_ec_pubkey_tweak_mul(CTX, &pubkey, ctmp) == 1);
    CHECK(secp256k1_ec_pubkey_negate(STATIC_CTX, &pubkey) == 1);
    CHECK(secp256k1_ec_pubkey_negate(CTX, &pubkey) == 1);
    CHECK_ILLEGAL(STATIC_CTX, secp256k1_ec_pubkey_negate(STATIC_CTX, &zero_pubkey));
    CHECK_ILLEGAL(CTX, secp256k1_ec_pubkey_negate(CTX, NULL));
    CHECK(secp256k1_ec_pubkey_tweak_mul(STATIC_CTX, &pubkey, ctmp) == 1);
}

static void run_static_context_tests(int use_prealloc) {
    /* Check that deprecated secp256k1_context_no_precomp is an alias to secp256k1_context_static. */
    CHECK(secp256k1_context_no_precomp == secp256k1_context_static);

    {
        unsigned char seed[32] = {0x17};

        /* Randomizing secp256k1_context_static is not supported. */
        CHECK_ILLEGAL(STATIC_CTX, secp256k1_context_randomize(STATIC_CTX, seed));
        CHECK_ILLEGAL(STATIC_CTX, secp256k1_context_randomize(STATIC_CTX, NULL));

        /* Destroying or cloning secp256k1_context_static is not supported. */
        if (use_prealloc) {
            CHECK_ILLEGAL(STATIC_CTX, secp256k1_context_preallocated_clone_size(STATIC_CTX));
            {
                secp256k1_context *my_static_ctx = malloc(sizeof(*STATIC_CTX));
                CHECK(my_static_ctx != NULL);
                memset(my_static_ctx, 0x2a, sizeof(*my_static_ctx));
                CHECK_ILLEGAL(STATIC_CTX, secp256k1_context_preallocated_clone(STATIC_CTX, my_static_ctx));
                CHECK(all_bytes_equal(my_static_ctx, 0x2a, sizeof(*my_static_ctx)));
                free(my_static_ctx);
            }
            CHECK_ILLEGAL_VOID(STATIC_CTX, secp256k1_context_preallocated_destroy(STATIC_CTX));
        } else {
            CHECK_ILLEGAL(STATIC_CTX, secp256k1_context_clone(STATIC_CTX));
            CHECK_ILLEGAL_VOID(STATIC_CTX, secp256k1_context_destroy(STATIC_CTX));
        }
    }

    {
        /* Verify that setting and resetting illegal callback works */
        int32_t dummy = 0;
        secp256k1_context_set_illegal_callback(STATIC_CTX, counting_callback_fn, &dummy);
        CHECK(STATIC_CTX->illegal_callback.fn == counting_callback_fn);
        CHECK(STATIC_CTX->illegal_callback.data == &dummy);
        secp256k1_context_set_illegal_callback(STATIC_CTX, NULL, NULL);
        CHECK(STATIC_CTX->illegal_callback.fn == secp256k1_default_illegal_callback_fn);
        CHECK(STATIC_CTX->illegal_callback.data == NULL);
    }
}

static void run_proper_context_tests(int use_prealloc) {
    int32_t dummy = 0;
    secp256k1_context *my_ctx, *my_ctx_fresh;
    void *my_ctx_prealloc = NULL;
    unsigned char seed[32] = {0x17};

    secp256k1_gej pubj;
    secp256k1_ge pub;
    secp256k1_scalar msg, key, nonce;
    secp256k1_scalar sigr, sigs;

    /* Fresh reference context for comparison */
    my_ctx_fresh = secp256k1_context_create(SECP256K1_CONTEXT_NONE);

    if (use_prealloc) {
        my_ctx_prealloc = malloc(secp256k1_context_preallocated_size(SECP256K1_CONTEXT_NONE));
        CHECK(my_ctx_prealloc != NULL);
        my_ctx = secp256k1_context_preallocated_create(my_ctx_prealloc, SECP256K1_CONTEXT_NONE);
    } else {
        my_ctx = secp256k1_context_create(SECP256K1_CONTEXT_NONE);
    }

    /* Randomize and reset randomization */
    CHECK(context_eq(my_ctx, my_ctx_fresh));
    CHECK(secp256k1_context_randomize(my_ctx, seed) == 1);
    CHECK(!context_eq(my_ctx, my_ctx_fresh));
    CHECK(secp256k1_context_randomize(my_ctx, NULL) == 1);
    CHECK(context_eq(my_ctx, my_ctx_fresh));

    /* set error callback (to a function that still aborts in case malloc() fails in secp256k1_context_clone() below) */
    secp256k1_context_set_error_callback(my_ctx, secp256k1_default_illegal_callback_fn, NULL);
    CHECK(my_ctx->error_callback.fn != secp256k1_default_error_callback_fn);
    CHECK(my_ctx->error_callback.fn == secp256k1_default_illegal_callback_fn);

    /* check if sizes for cloning are consistent */
    CHECK(secp256k1_context_preallocated_clone_size(my_ctx) == secp256k1_context_preallocated_size(SECP256K1_CONTEXT_NONE));

    /*** clone and destroy all of them to make sure cloning was complete ***/
    {
        secp256k1_context *ctx_tmp;

        if (use_prealloc) {
            /* clone into a non-preallocated context and then again into a new preallocated one. */
            ctx_tmp = my_ctx;
            my_ctx = secp256k1_context_clone(my_ctx);
            CHECK(context_eq(ctx_tmp, my_ctx));
            secp256k1_context_preallocated_destroy(ctx_tmp);

            free(my_ctx_prealloc);
            my_ctx_prealloc = malloc(secp256k1_context_preallocated_size(SECP256K1_CONTEXT_NONE));
            CHECK(my_ctx_prealloc != NULL);
            ctx_tmp = my_ctx;
            my_ctx = secp256k1_context_preallocated_clone(my_ctx, my_ctx_prealloc);
            CHECK(context_eq(ctx_tmp, my_ctx));
            secp256k1_context_destroy(ctx_tmp);
        } else {
            /* clone into a preallocated context and then again into a new non-preallocated one. */
            void *prealloc_tmp;

            prealloc_tmp = malloc(secp256k1_context_preallocated_size(SECP256K1_CONTEXT_NONE));
            CHECK(prealloc_tmp != NULL);
            ctx_tmp = my_ctx;
            my_ctx = secp256k1_context_preallocated_clone(my_ctx, prealloc_tmp);
            CHECK(context_eq(ctx_tmp, my_ctx));
            secp256k1_context_destroy(ctx_tmp);

            ctx_tmp = my_ctx;
            my_ctx = secp256k1_context_clone(my_ctx);
            CHECK(context_eq(ctx_tmp, my_ctx));
            secp256k1_context_preallocated_destroy(ctx_tmp);
            free(prealloc_tmp);
        }
    }

    /* Verify that the error callback makes it across the clone. */
    CHECK(my_ctx->error_callback.fn != secp256k1_default_error_callback_fn);
    CHECK(my_ctx->error_callback.fn == secp256k1_default_illegal_callback_fn);
    /* And that it resets back to default. */
    secp256k1_context_set_error_callback(my_ctx, NULL, NULL);
    CHECK(my_ctx->error_callback.fn == secp256k1_default_error_callback_fn);
    CHECK(context_eq(my_ctx, my_ctx_fresh));

    /* Verify that setting and resetting illegal callback works */
    secp256k1_context_set_illegal_callback(my_ctx, counting_callback_fn, &dummy);
    CHECK(my_ctx->illegal_callback.fn == counting_callback_fn);
    CHECK(my_ctx->illegal_callback.data == &dummy);
    secp256k1_context_set_illegal_callback(my_ctx, NULL, NULL);
    CHECK(my_ctx->illegal_callback.fn == secp256k1_default_illegal_callback_fn);
    CHECK(my_ctx->illegal_callback.data == NULL);
    CHECK(context_eq(my_ctx, my_ctx_fresh));

    /*** attempt to use them ***/
    testutil_random_scalar_order_test(&msg);
    testutil_random_scalar_order_test(&key);
    secp256k1_ecmult_gen(&my_ctx->ecmult_gen_ctx, &pubj, &key);
    secp256k1_ge_set_gej(&pub, &pubj);

    /* obtain a working nonce */
    do {
        testutil_random_scalar_order_test(&nonce);
    } while(!secp256k1_ecdsa_sig_sign(&my_ctx->ecmult_gen_ctx, &sigr, &sigs, &key, &msg, &nonce, NULL));

    /* try signing */
    CHECK(secp256k1_ecdsa_sig_sign(&my_ctx->ecmult_gen_ctx, &sigr, &sigs, &key, &msg, &nonce, NULL));

    /* try verifying */
    CHECK(secp256k1_ecdsa_sig_verify(&sigr, &sigs, &pub, &msg));

    /* cleanup */
    if (use_prealloc) {
        secp256k1_context_preallocated_destroy(my_ctx);
        free(my_ctx_prealloc);
    } else {
        secp256k1_context_destroy(my_ctx);
    }
    secp256k1_context_destroy(my_ctx_fresh);

    /* Defined as no-op. */
    secp256k1_context_destroy(NULL);
    secp256k1_context_preallocated_destroy(NULL);
}

static void run_scratch_tests(void) {
    const size_t adj_alloc = ((500 + ALIGNMENT - 1) / ALIGNMENT) * ALIGNMENT;

    size_t checkpoint;
    size_t checkpoint_2;
    secp256k1_scratch_space *scratch;
    secp256k1_scratch_space local_scratch;

    /* Test public API */
    scratch = secp256k1_scratch_space_create(CTX, 1000);
    CHECK(scratch != NULL);

    /* Test internal API */
    CHECK(secp256k1_scratch_max_allocation(&CTX->error_callback, scratch, 0) == 1000);
    CHECK(secp256k1_scratch_max_allocation(&CTX->error_callback, scratch, 1) == 1000 - (ALIGNMENT - 1));
    CHECK(scratch->alloc_size == 0);
    CHECK(scratch->alloc_size % ALIGNMENT == 0);

    /* Allocating 500 bytes succeeds */
    checkpoint = secp256k1_scratch_checkpoint(&CTX->error_callback, scratch);
    CHECK(secp256k1_scratch_alloc(&CTX->error_callback, scratch, 500) != NULL);
    CHECK(secp256k1_scratch_max_allocation(&CTX->error_callback, scratch, 0) == 1000 - adj_alloc);
    CHECK(secp256k1_scratch_max_allocation(&CTX->error_callback, scratch, 1) == 1000 - adj_alloc - (ALIGNMENT - 1));
    CHECK(scratch->alloc_size != 0);
    CHECK(scratch->alloc_size % ALIGNMENT == 0);

    /* Allocating another 501 bytes fails */
    CHECK(secp256k1_scratch_alloc(&CTX->error_callback, scratch, 501) == NULL);
    CHECK(secp256k1_scratch_max_allocation(&CTX->error_callback, scratch, 0) == 1000 - adj_alloc);
    CHECK(secp256k1_scratch_max_allocation(&CTX->error_callback, scratch, 1) == 1000 - adj_alloc - (ALIGNMENT - 1));
    CHECK(scratch->alloc_size != 0);
    CHECK(scratch->alloc_size % ALIGNMENT == 0);

    /* ...but it succeeds once we apply the checkpoint to undo it */
    secp256k1_scratch_apply_checkpoint(&CTX->error_callback, scratch, checkpoint);
    CHECK(scratch->alloc_size == 0);
    CHECK(secp256k1_scratch_max_allocation(&CTX->error_callback, scratch, 0) == 1000);
    CHECK(secp256k1_scratch_alloc(&CTX->error_callback, scratch, 500) != NULL);
    CHECK(scratch->alloc_size != 0);

    /* try to apply a bad checkpoint */
    checkpoint_2 = secp256k1_scratch_checkpoint(&CTX->error_callback, scratch);
    secp256k1_scratch_apply_checkpoint(&CTX->error_callback, scratch, checkpoint);
    CHECK_ERROR_VOID(CTX, secp256k1_scratch_apply_checkpoint(&CTX->error_callback, scratch, checkpoint_2)); /* checkpoint_2 is after checkpoint */
    CHECK_ERROR_VOID(CTX, secp256k1_scratch_apply_checkpoint(&CTX->error_callback, scratch, (size_t) -1)); /* this is just wildly invalid */

    /* try to use badly initialized scratch space */
    secp256k1_scratch_space_destroy(CTX, scratch);
    memset(&local_scratch, 0, sizeof(local_scratch));
    scratch = &local_scratch;
    CHECK_ERROR(CTX, secp256k1_scratch_max_allocation(&CTX->error_callback, scratch, 0));
    CHECK_ERROR(CTX, secp256k1_scratch_alloc(&CTX->error_callback, scratch, 500));
    CHECK_ERROR_VOID(CTX, secp256k1_scratch_space_destroy(CTX, scratch));

    /* Test that large integers do not wrap around in a bad way */
    scratch = secp256k1_scratch_space_create(CTX, 1000);
    /* Try max allocation with a large number of objects. Only makes sense if
     * ALIGNMENT is greater than 1 because otherwise the objects take no extra
     * space. */
    CHECK(ALIGNMENT <= 1 || !secp256k1_scratch_max_allocation(&CTX->error_callback, scratch, (SIZE_MAX / (ALIGNMENT - 1)) + 1));
    /* Try allocating SIZE_MAX to test wrap around which only happens if
     * ALIGNMENT > 1, otherwise it returns NULL anyway because the scratch
     * space is too small. */
    CHECK(secp256k1_scratch_alloc(&CTX->error_callback, scratch, SIZE_MAX) == NULL);
    secp256k1_scratch_space_destroy(CTX, scratch);

    /* cleanup */
    secp256k1_scratch_space_destroy(CTX, NULL); /* no-op */
}

static void run_ctz_tests(void) {
    static const uint32_t b32[] = {1, 0xffffffff, 0x5e56968f, 0xe0d63129};
    static const uint64_t b64[] = {1, 0xffffffffffffffff, 0xbcd02462139b3fc3, 0x98b5f80c769693ef};
    int shift;
    unsigned i;
    for (i = 0; i < sizeof(b32) / sizeof(b32[0]); ++i) {
        for (shift = 0; shift < 32; ++shift) {
            CHECK(secp256k1_ctz32_var_debruijn(b32[i] << shift) == shift);
            CHECK(secp256k1_ctz32_var(b32[i] << shift) == shift);
        }
    }
    for (i = 0; i < sizeof(b64) / sizeof(b64[0]); ++i) {
        for (shift = 0; shift < 64; ++shift) {
            CHECK(secp256k1_ctz64_var_debruijn(b64[i] << shift) == shift);
            CHECK(secp256k1_ctz64_var(b64[i] << shift) == shift);
        }
    }
}

/***** HASH TESTS *****/

static void run_sha256_known_output_tests(void) {
    static const char *inputs[] = {
        "", "abc", "message digest", "secure hash algorithm", "SHA256 is considered to be safe",
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
        "For this sample, this 63-byte string will be used as input data",
        "This is exactly 64 bytes long, not counting the terminating byte",
        "aaaaa",
    };
    static const unsigned int repeat[] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1000000/5
    };
    static const unsigned char outputs[][32] = {
        {0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14, 0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24, 0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c, 0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55},
        {0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea, 0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23, 0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c, 0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad},
        {0xf7, 0x84, 0x6f, 0x55, 0xcf, 0x23, 0xe1, 0x4e, 0xeb, 0xea, 0xb5, 0xb4, 0xe1, 0x55, 0x0c, 0xad, 0x5b, 0x50, 0x9e, 0x33, 0x48, 0xfb, 0xc4, 0xef, 0xa3, 0xa1, 0x41, 0x3d, 0x39, 0x3c, 0xb6, 0x50},
        {0xf3, 0x0c, 0xeb, 0x2b, 0xb2, 0x82, 0x9e, 0x79, 0xe4, 0xca, 0x97, 0x53, 0xd3, 0x5a, 0x8e, 0xcc, 0x00, 0x26, 0x2d, 0x16, 0x4c, 0xc0, 0x77, 0x08, 0x02, 0x95, 0x38, 0x1c, 0xbd, 0x64, 0x3f, 0x0d},
        {0x68, 0x19, 0xd9, 0x15, 0xc7, 0x3f, 0x4d, 0x1e, 0x77, 0xe4, 0xe1, 0xb5, 0x2d, 0x1f, 0xa0, 0xf9, 0xcf, 0x9b, 0xea, 0xea, 0xd3, 0x93, 0x9f, 0x15, 0x87, 0x4b, 0xd9, 0x88, 0xe2, 0xa2, 0x36, 0x30},
        {0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8, 0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39, 0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67, 0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1},
        {0xf0, 0x8a, 0x78, 0xcb, 0xba, 0xee, 0x08, 0x2b, 0x05, 0x2a, 0xe0, 0x70, 0x8f, 0x32, 0xfa, 0x1e, 0x50, 0xc5, 0xc4, 0x21, 0xaa, 0x77, 0x2b, 0xa5, 0xdb, 0xb4, 0x06, 0xa2, 0xea, 0x6b, 0xe3, 0x42},
        {0xab, 0x64, 0xef, 0xf7, 0xe8, 0x8e, 0x2e, 0x46, 0x16, 0x5e, 0x29, 0xf2, 0xbc, 0xe4, 0x18, 0x26, 0xbd, 0x4c, 0x7b, 0x35, 0x52, 0xf6, 0xb3, 0x82, 0xa9, 0xe7, 0xd3, 0xaf, 0x47, 0xc2, 0x45, 0xf8},
        {0xcd, 0xc7, 0x6e, 0x5c, 0x99, 0x14, 0xfb, 0x92, 0x81, 0xa1, 0xc7, 0xe2, 0x84, 0xd7, 0x3e, 0x67, 0xf1, 0x80, 0x9a, 0x48, 0xa4, 0x97, 0x20, 0x0e, 0x04, 0x6d, 0x39, 0xcc, 0xc7, 0x11, 0x2c, 0xd0},
    };
    unsigned int i, ninputs;

    /* Skip last input vector for low iteration counts */
    ninputs = sizeof(inputs)/sizeof(inputs[0]) - 1;
    CONDITIONAL_TEST(16, "run_sha256_known_output_tests 1000000") ninputs++;

    for (i = 0; i < ninputs; i++) {
        unsigned char out[32];
        secp256k1_sha256 hasher;
        unsigned int j;
        /* 1. Run: simply write the input bytestrings */
        j = repeat[i];
        secp256k1_sha256_initialize(&hasher);
        while (j > 0) {
            secp256k1_sha256_write(&hasher, (const unsigned char*)(inputs[i]), strlen(inputs[i]));
            j--;
        }
        secp256k1_sha256_finalize(&hasher, out);
        CHECK(secp256k1_memcmp_var(out, outputs[i], 32) == 0);
        /* 2. Run: split the input bytestrings randomly before writing */
        if (strlen(inputs[i]) > 0) {
            int split = testrand_int(strlen(inputs[i]));
            secp256k1_sha256_initialize(&hasher);
            j = repeat[i];
            while (j > 0) {
                secp256k1_sha256_write(&hasher, (const unsigned char*)(inputs[i]), split);
                secp256k1_sha256_write(&hasher, (const unsigned char*)(inputs[i] + split), strlen(inputs[i]) - split);
                j--;
            }
            secp256k1_sha256_finalize(&hasher, out);
            CHECK(secp256k1_memcmp_var(out, outputs[i], 32) == 0);
        }
    }
}

/** SHA256 counter tests

The tests verify that the SHA256 counter doesn't wrap around at message length
2^i bytes for i = 20, ..., 33. This wide range aims at being independent of the
implementation of the counter and it catches multiple natural 32-bit overflows
(e.g., counting bits, counting bytes, counting blocks, ...).

The test vectors have been generated using following Python script which relies
on https://github.com/cloudtools/sha256/ (v0.3 on Python v3.10.2).

```
from sha256 import sha256
from copy import copy

def midstate_c_definition(hasher):
    ret  = '    {{0x' + hasher.state[0].hex('_', 4).replace('_', ', 0x') + '},\n'
    ret += '    {0x00}, ' + str(hex(hasher.state[1])) + '}'
    return ret

def output_c_literal(hasher):
    return '{0x' + hasher.digest().hex('_').replace('_', ', 0x') + '}'

MESSAGE = b'abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno'
assert(len(MESSAGE) == 64)
BYTE_BOUNDARIES = [(2**b)//len(MESSAGE) - 1 for b in range(20, 34)]

midstates = []
digests = []
hasher = sha256()
for i in range(BYTE_BOUNDARIES[-1] + 1):
    if i in BYTE_BOUNDARIES:
        midstates.append(midstate_c_definition(hasher))
        hasher_copy = copy(hasher)
        hasher_copy.update(MESSAGE)
        digests.append(output_c_literal(hasher_copy))
    hasher.update(MESSAGE)

for x in midstates:
    print(x + ',')

for x in digests:
    print(x + ',')
```
*/
static void run_sha256_counter_tests(void) {
    static const char *input = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno";
    static const secp256k1_sha256 midstates[] = {
        {{0xa2b5c8bb, 0x26c88bb3, 0x2abdc3d2, 0x9def99a3, 0xdfd21a6e, 0x41fe585b, 0x7ef2c440, 0x2b79adda},
         {0x00}, 0xfffc0},
        {{0xa0d29445, 0x9287de66, 0x76aabd71, 0x41acd765, 0x0c7528b4, 0x84e14906, 0x942faec6, 0xcc5a7b26},
         {0x00}, 0x1fffc0},
        {{0x50449526, 0xb9f1d657, 0xa0fc13e9, 0x50860f10, 0xa550c431, 0x3fbc97c1, 0x7bbb2d89, 0xdb67bac1},
         {0x00}, 0x3fffc0},
        {{0x54a6efdc, 0x46762e7b, 0x88bfe73f, 0xbbd149c7, 0x41620c43, 0x1168da7b, 0x2c5960f9, 0xeccffda6},
         {0x00}, 0x7fffc0},
        {{0x2515a8f5, 0x5faa2977, 0x3a850486, 0xac858cad, 0x7b7276ee, 0x235c0385, 0xc53a157c, 0x7cb3e69c},
         {0x00}, 0xffffc0},
        {{0x34f39828, 0x409fedb7, 0x4bbdd0fb, 0x3b643634, 0x7806bf2e, 0xe0d1b713, 0xca3f2e1e, 0xe38722c2},
         {0x00}, 0x1ffffc0},
        {{0x389ef5c5, 0x38c54167, 0x8f5d56ab, 0x582a75cc, 0x8217caef, 0xf10947dd, 0x6a1998a8, 0x048f0b8c},
         {0x00}, 0x3ffffc0},
        {{0xd6c3f394, 0x0bee43b9, 0x6783f497, 0x29fa9e21, 0x6ce491c1, 0xa81fe45e, 0x2fc3859a, 0x269012d0},
         {0x00}, 0x7ffffc0},
        {{0x6dd3c526, 0x44d88aa0, 0x806a1bae, 0xfbcc0d32, 0x9d6144f3, 0x9d2bd757, 0x9851a957, 0xb50430ad},
         {0x00}, 0xfffffc0},
        {{0x2add4021, 0xdfe8a9e6, 0xa56317c6, 0x7a15f5bb, 0x4a48aacd, 0x5d368414, 0x4f00e6f0, 0xd9355023},
         {0x00}, 0x1fffffc0},
        {{0xb66666b4, 0xdbeac32b, 0x0ea351ae, 0xcba9da46, 0x6278b874, 0x8c508e23, 0xe16ca776, 0x8465bac1},
         {0x00}, 0x3fffffc0},
        {{0xb6744789, 0x9cce87aa, 0xc4c478b7, 0xf38404d8, 0x2e38ba62, 0xa3f7019b, 0x50458fe7, 0x3047dbec},
         {0x00}, 0x7fffffc0},
        {{0x8b1297ba, 0xba261a80, 0x2ba1b0dd, 0xfbc67d6d, 0x61072c4e, 0x4b5a2a0f, 0x52872760, 0x2dfeb162},
         {0x00}, 0xffffffc0},
        {{0x24f33cf7, 0x41ad6583, 0x41c8ff5d, 0xca7ef35f, 0x50395756, 0x021b743e, 0xd7126cd7, 0xd037473a},
         {0x00}, 0x1ffffffc0},
    };
    static const unsigned char outputs[][32] = {
        {0x0e, 0x83, 0xe2, 0xc9, 0x4f, 0xb2, 0xb8, 0x2b, 0x89, 0x06, 0x92, 0x78, 0x04, 0x03, 0x48, 0x5c, 0x48, 0x44, 0x67, 0x61, 0x77, 0xa4, 0xc7, 0x90, 0x9e, 0x92, 0x55, 0x10, 0x05, 0xfe, 0x39, 0x15},
        {0x1d, 0x1e, 0xd7, 0xb8, 0xa3, 0xa7, 0x8a, 0x79, 0xfd, 0xa0, 0x05, 0x08, 0x9c, 0xeb, 0xf0, 0xec, 0x67, 0x07, 0x9f, 0x8e, 0x3c, 0x0d, 0x8e, 0xf9, 0x75, 0x55, 0x13, 0xc1, 0xe8, 0x77, 0xf8, 0xbb},
        {0x66, 0x95, 0x6c, 0xc9, 0xe0, 0x39, 0x65, 0xb6, 0xb0, 0x05, 0xd1, 0xaf, 0xaf, 0xf3, 0x1d, 0xb9, 0xa4, 0xda, 0x6f, 0x20, 0xcd, 0x3a, 0xae, 0x64, 0xc2, 0xdb, 0xee, 0xf5, 0xb8, 0x8d, 0x57, 0x0e},
        {0x3c, 0xbb, 0x1c, 0x12, 0x5e, 0x17, 0xfd, 0x54, 0x90, 0x45, 0xa7, 0x7b, 0x61, 0x6c, 0x1d, 0xfe, 0xe6, 0xcc, 0x7f, 0xee, 0xcf, 0xef, 0x33, 0x35, 0x50, 0x62, 0x16, 0x70, 0x2f, 0x87, 0xc3, 0xc9},
        {0x53, 0x4d, 0xa8, 0xe7, 0x1e, 0x98, 0x73, 0x8d, 0xd9, 0xa3, 0x54, 0xa5, 0x0e, 0x59, 0x2c, 0x25, 0x43, 0x6f, 0xaa, 0xa2, 0xf5, 0x21, 0x06, 0x3e, 0xc9, 0x82, 0x06, 0x94, 0x98, 0x72, 0x9d, 0xa7},
        {0xef, 0x7e, 0xe9, 0x6b, 0xd3, 0xe5, 0xb7, 0x41, 0x4c, 0xc8, 0xd3, 0x07, 0x52, 0x9a, 0x5a, 0x8b, 0x4e, 0x1e, 0x75, 0xa4, 0x17, 0x78, 0xc8, 0x36, 0xcd, 0xf8, 0x2e, 0xd9, 0x57, 0xe3, 0xd7, 0x07},
        {0x87, 0x16, 0xfb, 0xf9, 0xa5, 0xf8, 0xc4, 0x56, 0x2b, 0x48, 0x52, 0x8e, 0x2d, 0x30, 0x85, 0xb6, 0x4c, 0x56, 0xb5, 0xd1, 0x16, 0x9c, 0xcf, 0x32, 0x95, 0xad, 0x03, 0xe8, 0x05, 0x58, 0x06, 0x76},
        {0x75, 0x03, 0x80, 0x28, 0xf2, 0xa7, 0x63, 0x22, 0x1a, 0x26, 0x9c, 0x68, 0xe0, 0x58, 0xfc, 0x73, 0xeb, 0x42, 0xf6, 0x86, 0x16, 0x24, 0x4b, 0xbc, 0x24, 0xf7, 0x02, 0xc8, 0x3d, 0x90, 0xe2, 0xb0},
        {0xdf, 0x49, 0x0f, 0x15, 0x7b, 0x7d, 0xbf, 0xe0, 0xd4, 0xcf, 0x47, 0xc0, 0x80, 0x93, 0x4a, 0x61, 0xaa, 0x03, 0x07, 0x66, 0xb3, 0x38, 0x5d, 0xc8, 0xc9, 0x07, 0x61, 0xfb, 0x97, 0x10, 0x2f, 0xd8},
        {0x77, 0x19, 0x40, 0x56, 0x41, 0xad, 0xbc, 0x59, 0xda, 0x1e, 0xc5, 0x37, 0x14, 0x63, 0x7b, 0xfb, 0x79, 0xe2, 0x7a, 0xb1, 0x55, 0x42, 0x99, 0x42, 0x56, 0xfe, 0x26, 0x9d, 0x0f, 0x7e, 0x80, 0xc6},
        {0x50, 0xe7, 0x2a, 0x0e, 0x26, 0x44, 0x2f, 0xe2, 0x55, 0x2d, 0xc3, 0x93, 0x8a, 0xc5, 0x86, 0x58, 0x22, 0x8c, 0x0c, 0xbf, 0xb1, 0xd2, 0xca, 0x87, 0x2a, 0xe4, 0x35, 0x26, 0x6f, 0xcd, 0x05, 0x5e},
        {0xe4, 0x80, 0x6f, 0xdb, 0x3d, 0x7d, 0xba, 0xde, 0x50, 0x3f, 0xea, 0x00, 0x3d, 0x46, 0x59, 0x64, 0xfd, 0x58, 0x1c, 0xa1, 0xb8, 0x7d, 0x5f, 0xac, 0x94, 0x37, 0x9e, 0xa0, 0xc0, 0x9c, 0x93, 0x8b},
        {0x2c, 0xf3, 0xa9, 0xf6, 0x15, 0x25, 0x80, 0x70, 0x76, 0x99, 0x7d, 0xf1, 0xc3, 0x2f, 0xa3, 0x31, 0xff, 0x92, 0x35, 0x2e, 0x8d, 0x04, 0x13, 0x33, 0xd8, 0x0d, 0xdb, 0x4a, 0xf6, 0x8c, 0x03, 0x34},
        {0xec, 0x12, 0x24, 0x9f, 0x35, 0xa4, 0x29, 0x8b, 0x9e, 0x4a, 0x95, 0xf8, 0x61, 0xaf, 0x61, 0xc5, 0x66, 0x55, 0x3e, 0x3f, 0x2a, 0x98, 0xea, 0x71, 0x16, 0x6b, 0x1c, 0xd9, 0xe4, 0x09, 0xd2, 0x8e},
    };
    unsigned int i;
    for (i = 0; i < sizeof(midstates)/sizeof(midstates[0]); i++) {
        unsigned char out[32];
        secp256k1_sha256 hasher = midstates[i];
        secp256k1_sha256_write(&hasher, (const unsigned char*)input, strlen(input));
        secp256k1_sha256_finalize(&hasher, out);
        CHECK(secp256k1_memcmp_var(out, outputs[i], 32) == 0);
    }
}

/* Tests for the equality of two sha256 structs. This function only produces a
 * correct result if an integer multiple of 64 many bytes have been written
 * into the hash functions. This function is used by some module tests. */
static void test_sha256_eq(const secp256k1_sha256 *sha1, const secp256k1_sha256 *sha2) {
    /* Is buffer fully consumed? */
    CHECK((sha1->bytes & 0x3F) == 0);

    CHECK(sha1->bytes == sha2->bytes);
    CHECK(secp256k1_memcmp_var(sha1->s, sha2->s, sizeof(sha1->s)) == 0);
}

static void run_hmac_sha256_tests(void) {
    static const char *keys[6] = {
        "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b",
        "\x4a\x65\x66\x65",
        "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa",
        "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19",
        "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa",
        "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
    };
    static const char *inputs[6] = {
        "\x48\x69\x20\x54\x68\x65\x72\x65",
        "\x77\x68\x61\x74\x20\x64\x6f\x20\x79\x61\x20\x77\x61\x6e\x74\x20\x66\x6f\x72\x20\x6e\x6f\x74\x68\x69\x6e\x67\x3f",
        "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd",
        "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd",
        "\x54\x65\x73\x74\x20\x55\x73\x69\x6e\x67\x20\x4c\x61\x72\x67\x65\x72\x20\x54\x68\x61\x6e\x20\x42\x6c\x6f\x63\x6b\x2d\x53\x69\x7a\x65\x20\x4b\x65\x79\x20\x2d\x20\x48\x61\x73\x68\x20\x4b\x65\x79\x20\x46\x69\x72\x73\x74",
        "\x54\x68\x69\x73\x20\x69\x73\x20\x61\x20\x74\x65\x73\x74\x20\x75\x73\x69\x6e\x67\x20\x61\x20\x6c\x61\x72\x67\x65\x72\x20\x74\x68\x61\x6e\x20\x62\x6c\x6f\x63\x6b\x2d\x73\x69\x7a\x65\x20\x6b\x65\x79\x20\x61\x6e\x64\x20\x61\x20\x6c\x61\x72\x67\x65\x72\x20\x74\x68\x61\x6e\x20\x62\x6c\x6f\x63\x6b\x2d\x73\x69\x7a\x65\x20\x64\x61\x74\x61\x2e\x20\x54\x68\x65\x20\x6b\x65\x79\x20\x6e\x65\x65\x64\x73\x20\x74\x6f\x20\x62\x65\x20\x68\x61\x73\x68\x65\x64\x20\x62\x65\x66\x6f\x72\x65\x20\x62\x65\x69\x6e\x67\x20\x75\x73\x65\x64\x20\x62\x79\x20\x74\x68\x65\x20\x48\x4d\x41\x43\x20\x61\x6c\x67\x6f\x72\x69\x74\x68\x6d\x2e"
    };
    static const unsigned char outputs[6][32] = {
        {0xb0, 0x34, 0x4c, 0x61, 0xd8, 0xdb, 0x38, 0x53, 0x5c, 0xa8, 0xaf, 0xce, 0xaf, 0x0b, 0xf1, 0x2b, 0x88, 0x1d, 0xc2, 0x00, 0xc9, 0x83, 0x3d, 0xa7, 0x26, 0xe9, 0x37, 0x6c, 0x2e, 0x32, 0xcf, 0xf7},
        {0x5b, 0xdc, 0xc1, 0x46, 0xbf, 0x60, 0x75, 0x4e, 0x6a, 0x04, 0x24, 0x26, 0x08, 0x95, 0x75, 0xc7, 0x5a, 0x00, 0x3f, 0x08, 0x9d, 0x27, 0x39, 0x83, 0x9d, 0xec, 0x58, 0xb9, 0x64, 0xec, 0x38, 0x43},
        {0x77, 0x3e, 0xa9, 0x1e, 0x36, 0x80, 0x0e, 0x46, 0x85, 0x4d, 0xb8, 0xeb, 0xd0, 0x91, 0x81, 0xa7, 0x29, 0x59, 0x09, 0x8b, 0x3e, 0xf8, 0xc1, 0x22, 0xd9, 0x63, 0x55, 0x14, 0xce, 0xd5, 0x65, 0xfe},
        {0x82, 0x55, 0x8a, 0x38, 0x9a, 0x44, 0x3c, 0x0e, 0xa4, 0xcc, 0x81, 0x98, 0x99, 0xf2, 0x08, 0x3a, 0x85, 0xf0, 0xfa, 0xa3, 0xe5, 0x78, 0xf8, 0x07, 0x7a, 0x2e, 0x3f, 0xf4, 0x67, 0x29, 0x66, 0x5b},
        {0x60, 0xe4, 0x31, 0x59, 0x1e, 0xe0, 0xb6, 0x7f, 0x0d, 0x8a, 0x26, 0xaa, 0xcb, 0xf5, 0xb7, 0x7f, 0x8e, 0x0b, 0xc6, 0x21, 0x37, 0x28, 0xc5, 0x14, 0x05, 0x46, 0x04, 0x0f, 0x0e, 0xe3, 0x7f, 0x54},
        {0x9b, 0x09, 0xff, 0xa7, 0x1b, 0x94, 0x2f, 0xcb, 0x27, 0x63, 0x5f, 0xbc, 0xd5, 0xb0, 0xe9, 0x44, 0xbf, 0xdc, 0x63, 0x64, 0x4f, 0x07, 0x13, 0x93, 0x8a, 0x7f, 0x51, 0x53, 0x5c, 0x3a, 0x35, 0xe2}
    };
    int i;
    for (i = 0; i < 6; i++) {
        secp256k1_hmac_sha256 hasher;
        unsigned char out[32];
        secp256k1_hmac_sha256_initialize(&hasher, (const unsigned char*)(keys[i]), strlen(keys[i]));
        secp256k1_hmac_sha256_write(&hasher, (const unsigned char*)(inputs[i]), strlen(inputs[i]));
        secp256k1_hmac_sha256_finalize(&hasher, out);
        CHECK(secp256k1_memcmp_var(out, outputs[i], 32) == 0);
        if (strlen(inputs[i]) > 0) {
            int split = testrand_int(strlen(inputs[i]));
            secp256k1_hmac_sha256_initialize(&hasher, (const unsigned char*)(keys[i]), strlen(keys[i]));
            secp256k1_hmac_sha256_write(&hasher, (const unsigned char*)(inputs[i]), split);
            secp256k1_hmac_sha256_write(&hasher, (const unsigned char*)(inputs[i] + split), strlen(inputs[i]) - split);
            secp256k1_hmac_sha256_finalize(&hasher, out);
            CHECK(secp256k1_memcmp_var(out, outputs[i], 32) == 0);
        }
    }
}

static void run_rfc6979_hmac_sha256_tests(void) {
    static const unsigned char key1[65] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x00, 0x4b, 0xf5, 0x12, 0x2f, 0x34, 0x45, 0x54, 0xc5, 0x3b, 0xde, 0x2e, 0xbb, 0x8c, 0xd2, 0xb7, 0xe3, 0xd1, 0x60, 0x0a, 0xd6, 0x31, 0xc3, 0x85, 0xa5, 0xd7, 0xcc, 0xe2, 0x3c, 0x77, 0x85, 0x45, 0x9a, 0};
    static const unsigned char out1[3][32] = {
        {0x4f, 0xe2, 0x95, 0x25, 0xb2, 0x08, 0x68, 0x09, 0x15, 0x9a, 0xcd, 0xf0, 0x50, 0x6e, 0xfb, 0x86, 0xb0, 0xec, 0x93, 0x2c, 0x7b, 0xa4, 0x42, 0x56, 0xab, 0x32, 0x1e, 0x42, 0x1e, 0x67, 0xe9, 0xfb},
        {0x2b, 0xf0, 0xff, 0xf1, 0xd3, 0xc3, 0x78, 0xa2, 0x2d, 0xc5, 0xde, 0x1d, 0x85, 0x65, 0x22, 0x32, 0x5c, 0x65, 0xb5, 0x04, 0x49, 0x1a, 0x0c, 0xbd, 0x01, 0xcb, 0x8f, 0x3a, 0xa6, 0x7f, 0xfd, 0x4a},
        {0xf5, 0x28, 0xb4, 0x10, 0xcb, 0x54, 0x1f, 0x77, 0x00, 0x0d, 0x7a, 0xfb, 0x6c, 0x5b, 0x53, 0xc5, 0xc4, 0x71, 0xea, 0xb4, 0x3e, 0x46, 0x6d, 0x9a, 0xc5, 0x19, 0x0c, 0x39, 0xc8, 0x2f, 0xd8, 0x2e}
    };

    static const unsigned char key2[64] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14, 0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24, 0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c, 0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55};
    static const unsigned char out2[3][32] = {
        {0x9c, 0x23, 0x6c, 0x16, 0x5b, 0x82, 0xae, 0x0c, 0xd5, 0x90, 0x65, 0x9e, 0x10, 0x0b, 0x6b, 0xab, 0x30, 0x36, 0xe7, 0xba, 0x8b, 0x06, 0x74, 0x9b, 0xaf, 0x69, 0x81, 0xe1, 0x6f, 0x1a, 0x2b, 0x95},
        {0xdf, 0x47, 0x10, 0x61, 0x62, 0x5b, 0xc0, 0xea, 0x14, 0xb6, 0x82, 0xfe, 0xee, 0x2c, 0x9c, 0x02, 0xf2, 0x35, 0xda, 0x04, 0x20, 0x4c, 0x1d, 0x62, 0xa1, 0x53, 0x6c, 0x6e, 0x17, 0xae, 0xd7, 0xa9},
        {0x75, 0x97, 0x88, 0x7c, 0xbd, 0x76, 0x32, 0x1f, 0x32, 0xe3, 0x04, 0x40, 0x67, 0x9a, 0x22, 0xcf, 0x7f, 0x8d, 0x9d, 0x2e, 0xac, 0x39, 0x0e, 0x58, 0x1f, 0xea, 0x09, 0x1c, 0xe2, 0x02, 0xba, 0x94}
    };

    secp256k1_rfc6979_hmac_sha256 rng;
    unsigned char out[32];
    int i;

    secp256k1_rfc6979_hmac_sha256_initialize(&rng, key1, 64);
    for (i = 0; i < 3; i++) {
        secp256k1_rfc6979_hmac_sha256_generate(&rng, out, 32);
        CHECK(secp256k1_memcmp_var(out, out1[i], 32) == 0);
    }
    secp256k1_rfc6979_hmac_sha256_finalize(&rng);

    secp256k1_rfc6979_hmac_sha256_initialize(&rng, key1, 65);
    for (i = 0; i < 3; i++) {
        secp256k1_rfc6979_hmac_sha256_generate(&rng, out, 32);
        CHECK(secp256k1_memcmp_var(out, out1[i], 32) != 0);
    }
    secp256k1_rfc6979_hmac_sha256_finalize(&rng);

    secp256k1_rfc6979_hmac_sha256_initialize(&rng, key2, 64);
    for (i = 0; i < 3; i++) {
        secp256k1_rfc6979_hmac_sha256_generate(&rng, out, 32);
        CHECK(secp256k1_memcmp_var(out, out2[i], 32) == 0);
    }
    secp256k1_rfc6979_hmac_sha256_finalize(&rng);
}

static void run_tagged_sha256_tests(void) {
    unsigned char tag[32] = { 0 };
    unsigned char msg[32] = { 0 };
    unsigned char hash32[32];
    unsigned char hash_expected[32] = {
        0x04, 0x7A, 0x5E, 0x17, 0xB5, 0x86, 0x47, 0xC1,
        0x3C, 0xC6, 0xEB, 0xC0, 0xAA, 0x58, 0x3B, 0x62,
        0xFB, 0x16, 0x43, 0x32, 0x68, 0x77, 0x40, 0x6C,
        0xE2, 0x76, 0x55, 0x9A, 0x3B, 0xDE, 0x55, 0xB3
    };

    /* API test */
    CHECK(secp256k1_tagged_sha256(CTX, hash32, tag, sizeof(tag), msg, sizeof(msg)) == 1);
    CHECK_ILLEGAL(CTX, secp256k1_tagged_sha256(CTX, NULL, tag, sizeof(tag), msg, sizeof(msg)));
    CHECK_ILLEGAL(CTX, secp256k1_tagged_sha256(CTX, hash32, NULL, 0, msg, sizeof(msg)));
    CHECK_ILLEGAL(CTX, secp256k1_tagged_sha256(CTX, hash32, tag, sizeof(tag), NULL, 0));

    /* Static test vector */
    memcpy(tag, "tag", 3);
    memcpy(msg, "msg", 3);
    CHECK(secp256k1_tagged_sha256(CTX, hash32, tag, 3, msg, 3) == 1);
    CHECK(secp256k1_memcmp_var(hash32, hash_expected, sizeof(hash32)) == 0);
}

/***** MODINV TESTS *****/

/* Compute the modular inverse of (odd) x mod 2^64. */
static uint64_t modinv2p64(uint64_t x) {
    /* If w = 1/x mod 2^(2^L), then w*(2 - w*x) = 1/x mod 2^(2^(L+1)). See
     * Hacker's Delight second edition, Henry S. Warren, Jr., pages 245-247 for
     * why. Start with L=0, for which it is true for every odd x that
     * 1/x=1 mod 2. Iterating 6 times gives us 1/x mod 2^64. */
    int l;
    uint64_t w = 1;
    CHECK(x & 1);
    for (l = 0; l < 6; ++l) w *= (2 - w*x);
    return w;
}


/* compute out = (a*b) mod m; if b=NULL, treat b=1; if m=NULL, treat m=infinity.
 *
 * Out is a 512-bit number (represented as 32 uint16_t's in LE order). The other
 * arguments are 256-bit numbers (represented as 16 uint16_t's in LE order). */
static void mulmod256(uint16_t* out, const uint16_t* a, const uint16_t* b, const uint16_t* m) {
    uint16_t mul[32];
    uint64_t c = 0;
    int i, j;
    int m_bitlen = 0;
    int mul_bitlen = 0;

    if (b != NULL) {
        /* Compute the product of a and b, and put it in mul. */
        for (i = 0; i < 32; ++i) {
            for (j = i <= 15 ? 0 : i - 15; j <= i && j <= 15; j++) {
                c += (uint64_t)a[j] * b[i - j];
            }
            mul[i] = c & 0xFFFF;
            c >>= 16;
        }
        CHECK(c == 0);

        /* compute the highest set bit in mul */
        for (i = 511; i >= 0; --i) {
            if ((mul[i >> 4] >> (i & 15)) & 1) {
                mul_bitlen = i;
                break;
            }
        }
    } else {
        /* if b==NULL, set mul=a. */
        memcpy(mul, a, 32);
        memset(mul + 16, 0, 32);
