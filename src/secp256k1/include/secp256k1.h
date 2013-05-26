#ifndef SECP256K1_H
#define SECP256K1_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/** Unless explicitly stated all pointer arguments must not be NULL.
 *
 * The following rules specify the order of arguments in API calls:
 *
 * 1. Context pointers go first, followed by output arguments, combined
 *    output/input arguments, and finally input-only arguments.
 * 2. Array lengths always immediately follow the argument whose length
 *    they describe, even if this violates rule 1.
 * 3. Within the OUT/OUTIN/IN groups, pointers to data that is typically generated
 *    later go first. This means: signatures, public nonces, secret nonces,
 *    messages, public keys, secret keys, tweaks.
 * 4. Arguments that are not data pointers go last, from more complex to less
 *    complex: function pointers, algorithm names, messages, void pointers,
 *    counts, flags, booleans.
 * 5. Opaque data pointers follow the function pointer they are to be passed to.
 */

/** Opaque data structure that holds context information
 *
 *  The primary purpose of context objects is to store randomization data for
 *  enhanced protection against side-channel leakage. This protection is only
 *  effective if the context is randomized after its creation. See
 *  secp256k1_context_create for creation of contexts and
 *  secp256k1_context_randomize for randomization.
 *
 *  A secondary purpose of context objects is to store pointers to callback
 *  functions that the library will call when certain error states arise. See
 *  secp256k1_context_set_error_callback as well as
 *  secp256k1_context_set_illegal_callback for details. Future library versions
 *  may use context objects for additional purposes.
 *
 *  A constructed context can safely be used from multiple threads
 *  simultaneously, but API calls that take a non-const pointer to a context
 *  need exclusive access to it. In particular this is the case for
 *  secp256k1_context_destroy, secp256k1_context_preallocated_destroy,
 *  and secp256k1_context_randomize.
 *
 *  Regarding randomization, either do it once at creation time (in which case
 *  you do not need any locking for the other calls), or use a read-write lock.
 */
typedef struct secp256k1_context_struct secp256k1_context;

/** Opaque data structure that holds rewritable "scratch space"
 *
 *  The purpose of this structure is to replace dynamic memory allocations,
 *  because we target architectures where this may not be available. It is
 *  essentially a resizable (within specified parameters) block of bytes,
 *  which is initially created either by memory allocation or TODO as a pointer
 *  into some fixed rewritable space.
 *
 *  Unlike the context object, this cannot safely be shared between threads
 *  without additional synchronization logic.
 */
typedef struct secp256k1_scratch_space_struct secp256k1_scratch_space;

/** Opaque data structure that holds a parsed and valid public key.
 *
 *  The exact representation of data inside is implementation defined and not
 *  guaranteed to be portable between different platforms or versions. It is
 *  however guaranteed to be 64 bytes in size, and can be safely copied/moved.
 *  If you need to convert to a format suitable for storage or transmission,
 *  use secp256k1_ec_pubkey_serialize and secp256k1_ec_pubkey_parse. To
 *  compare keys, use secp256k1_ec_pubkey_cmp.
 */
typedef struct {
    unsigned char data[64];
} secp256k1_pubkey;

/** Opaque data structured that holds a parsed ECDSA signature.
 *
 *  The exact representation of data inside is implementation defined and not
 *  guaranteed to be portable between different platforms or versions. It is
 *  however guaranteed to be 64 bytes in size, and can be safely copied/moved.
 *  If you need to convert to a format suitable for storage, transmission, or
 *  comparison, use the secp256k1_ecdsa_signature_serialize_* and
 *  secp256k1_ecdsa_signature_parse_* functions.
 */
typedef struct {
    unsigned char data[64];
} secp256k1_ecdsa_signature;

/** A pointer to a function to deterministically generate a nonce.
 *
