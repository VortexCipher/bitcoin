# Copyright (c) 2019-2020 Pieter Wuille
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test-only secp256k1 elliptic curve protocols implementation

WARNING: This code is slow, uses bad randomness, does not properly protect
keys, and is trivially vulnerable to side channel attacks. Do not use for
anything but tests."""
import csv
import hashlib
import hmac
import os
import random
import unittest

from test_framework.crypto import secp256k1
from test_framework.util import random_bitflip

# Point with no known discrete log.
H_POINT = "50929b74c1a04954b78b4b6035e97a5e078a5a0f28ec96d547bfee9ace803ac0"

# Order of the secp256k1 curve
ORDER = secp256k1.GE.ORDER

def TaggedHash(tag, data):
    ss = hashlib.sha256(tag.encode('utf-8')).digest()
    ss += ss
    ss += data
    return hashlib.sha256(ss).digest()


class ECPubKey:
    """A secp256k1 public key"""

