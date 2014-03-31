#!/usr/bin/env python3
# Copyright (c) 2019-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
# Test Taproot softfork (BIPs 340-342)

from test_framework.blocktools import (
    COINBASE_MATURITY,
    create_coinbase,
    create_block,
    add_witness_commitment,
    MAX_BLOCK_SIGOPS_WEIGHT,
)
from test_framework.messages import (
    COutPoint,
    CTransaction,
    CTxIn,
    CTxInWitness,
    CTxOut,
    SEQUENCE_FINAL,
    tx_from_hex,
    WITNESS_SCALE_FACTOR,
)
from test_framework.script import (
    ANNEX_TAG,
    BIP341_sha_amounts,
    BIP341_sha_outputs,
    BIP341_sha_prevouts,
    BIP341_sha_scriptpubkeys,
    BIP341_sha_sequences,
    CScript,
    CScriptNum,
    CScriptOp,
    hash256,
    LEAF_VERSION_TAPSCRIPT,
    LegacySignatureMsg,
    LOCKTIME_THRESHOLD,
    MAX_SCRIPT_ELEMENT_SIZE,
    OP_0,
    OP_1,
    OP_2,
    OP_3,
    OP_4,
    OP_5,
    OP_6,
    OP_7,
    OP_8,
    OP_9,
    OP_10,
    OP_11,
    OP_12,
    OP_16,
    OP_2DROP,
    OP_2DUP,
    OP_CHECKMULTISIG,
    OP_CHECKMULTISIGVERIFY,
    OP_CHECKSIG,
    OP_CHECKSIGADD,
    OP_CHECKSIGVERIFY,
    OP_CODESEPARATOR,
    OP_DROP,
    OP_DUP,
    OP_ELSE,
    OP_ENDIF,
    OP_EQUAL,
    OP_EQUALVERIFY,
    OP_IF,
    OP_NOP,
    OP_NOT,
    OP_NOTIF,
    OP_PUSHDATA1,
    OP_RETURN,
    OP_SWAP,
    OP_VERIFY,
    SIGHASH_DEFAULT,
    SIGHASH_ALL,
    SIGHASH_NONE,
    SIGHASH_SINGLE,
    SIGHASH_ANYONECANPAY,
    SegwitV0SignatureMsg,
    TaggedHash,
    TaprootSignatureMsg,
    is_op_success,
    taproot_construct,
)
from test_framework.script_util import (
    key_to_p2pk_script,
    key_to_p2pkh_script,
    key_to_p2wpkh_script,
    keyhash_to_p2pkh_script,
    script_to_p2sh_script,
    script_to_p2wsh_script,
)
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_raises_rpc_error,
    assert_equal,
)
from test_framework.wallet_util import generate_keypair
from test_framework.key import (
    generate_privkey,
    compute_xonly_pubkey,
    sign_schnorr,
    tweak_add_privkey,
    ECKey,
)
from test_framework.crypto import secp256k1
from test_framework.address import (
    hash160,
    program_to_witness,
)
from collections import OrderedDict, namedtuple
import json
import hashlib
import os
import random

# Whether or not to output generated test vectors, in JSON format.
GEN_TEST_VECTORS = False

# === Framework for building spending transactions. ===
#
# The computation is represented as a "context" dict, whose entries store potentially-unevaluated expressions that
# refer to lower-level ones. By overwriting these expression, many aspects - both high and low level - of the signing
# process can be overridden.
#
# Specifically, a context object is a dict that maps names to compositions of:
# - values
# - lists of values
# - callables which, when fed the context object as argument, produce any of these
#
# The DEFAULT_CONTEXT object specifies a standard signing process, with many overridable knobs.
#
# The get(ctx, name) function can evaluate a name, and cache its result in the context.
# getter(name) can be used to construct a callable that evaluates name. For example:
#
#   ctx1 = {**DEFAULT_CONTEXT, inputs=[getter("sign"), b'\x01']}
#
# creates a context where the script inputs are a signature plus the bytes 0x01.
#
# override(expr, name1=expr1, name2=expr2, ...) can be used to cause an expression to be evaluated in a selectively
# modified context. For example:
#
#   ctx2 = {**DEFAULT_CONTEXT, sighash=override(default_sighash, hashtype=SIGHASH_DEFAULT)}
#
# creates a context ctx2 where the sighash is modified to use hashtype=SIGHASH_DEFAULT. This differs from
#
#   ctx3 = {**DEFAULT_CONTEXT, hashtype=SIGHASH_DEFAULT}
#
# in that ctx3 will globally use hashtype=SIGHASH_DEFAULT (including in the hashtype byte appended to the signature)
# while ctx2 only uses the modified hashtype inside the sighash calculation.

def deep_eval(ctx, expr):
    """Recursively replace any callables c in expr (including inside lists) with c(ctx)."""
    while callable(expr):
        expr = expr(ctx)
    if isinstance(expr, list):
        expr = [deep_eval(ctx, x) for x in expr]
    return expr

# Data type to represent fully-evaluated expressions in a context dict (so we can avoid reevaluating them).
Final = namedtuple("Final", "value")

def get(ctx, name):
    """Evaluate name in context ctx."""
    assert name in ctx, "Missing '%s' in context" % name
    expr = ctx[name]
    if not isinstance(expr, Final):
        # Evaluate and cache the result.
        expr = Final(deep_eval(ctx, expr))
        ctx[name] = expr
    return expr.value

def getter(name):
    """Return a callable that evaluates name in its passed context."""
    return lambda ctx: get(ctx, name)
