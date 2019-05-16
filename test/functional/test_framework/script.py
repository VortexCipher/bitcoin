#!/usr/bin/env python3
# Copyright (c) 2015-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Functionality to build scripts, as well as signature hash functions.

This file is modified from python-bitcoinlib.
"""

from collections import namedtuple
import unittest

from .key import TaggedHash, tweak_add_pubkey, compute_xonly_pubkey

from .messages import (
    CTransaction,
    CTxOut,
    hash256,
    ser_string,
    ser_uint256,
    sha256,
    uint256_from_str,
)

from .crypto.ripemd160 import ripemd160

MAX_SCRIPT_ELEMENT_SIZE = 520
MAX_PUBKEYS_PER_MULTI_A = 999
LOCKTIME_THRESHOLD = 500000000
ANNEX_TAG = 0x50

LEAF_VERSION_TAPSCRIPT = 0xc0

def hash160(s):
    return ripemd160(sha256(s))

def bn2vch(v):
    """Convert number to bitcoin-specific little endian format."""
    # We need v.bit_length() bits, plus a sign bit for every nonzero number.
    n_bits = v.bit_length() + (v != 0)
    # The number of bytes for that is:
    n_bytes = (n_bits + 7) // 8
    # Convert number to absolute value + sign in top bit.
    encoded_v = 0 if v == 0 else abs(v) | ((v < 0) << (n_bytes * 8 - 1))
    # Serialize to bytes
    return encoded_v.to_bytes(n_bytes, 'little')

class CScriptOp(int):
    """A single script opcode"""
    __slots__ = ()

    @staticmethod
    def encode_op_pushdata(d):
        """Encode a PUSHDATA op, returning bytes"""
        if len(d) < 0x4c:
            return b'' + bytes([len(d)]) + d  # OP_PUSHDATA
        elif len(d) <= 0xff:
            return b'\x4c' + bytes([len(d)]) + d  # OP_PUSHDATA1
        elif len(d) <= 0xffff:
            return b'\x4d' + len(d).to_bytes(2, "little") + d  # OP_PUSHDATA2
        elif len(d) <= 0xffffffff:
            return b'\x4e' + len(d).to_bytes(4, "little") + d  # OP_PUSHDATA4
        else:
            raise ValueError("Data too long to encode in a PUSHDATA op")

    @staticmethod
    def encode_op_n(n):
        """Encode a small integer op, returning an opcode"""
        if not (0 <= n <= 16):
            raise ValueError('Integer must be in range 0 <= n <= 16, got %d' % n)

        if n == 0:
            return OP_0
        else:
            return CScriptOp(OP_1 + n - 1)

    def decode_op_n(self):
        """Decode a small integer opcode, returning an integer"""
        if self == OP_0:
            return 0

        if not (self == OP_0 or OP_1 <= self <= OP_16):
            raise ValueError('op %r is not an OP_N' % self)

        return int(self - OP_1 + 1)

    def is_small_int(self):
        """Return true if the op pushes a small integer to the stack"""
        if 0x51 <= self <= 0x60 or self == 0:
            return True
        else:
            return False

    def __str__(self):
