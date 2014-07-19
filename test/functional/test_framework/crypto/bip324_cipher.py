#!/usr/bin/env python3
# Copyright (c) 2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

"""Test-only implementation of ChaCha20 Poly1305 AEAD Construction in RFC 8439 and FSChaCha20Poly1305 for BIP 324

It is designed for ease of understanding, not performance.

WARNING: This code is slow and trivially vulnerable to side channel attacks. Do not use for
anything but tests.
"""

import unittest

from .chacha20 import chacha20_block, REKEY_INTERVAL
from .poly1305 import Poly1305


def pad16(x):
