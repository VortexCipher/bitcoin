#!/usr/bin/env python3
# Copyright (c) 2016-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Encode and decode Bitcoin addresses.

- base58 P2PKH and P2SH addresses.
- bech32 segwit v0 P2WPKH and P2WSH addresses.
- bech32m segwit v1 P2TR addresses."""

import enum
import unittest

from .script import (
    CScript,
    OP_0,
    OP_TRUE,
    hash160,
    hash256,
    sha256,
    taproot_construct,
)
from .util import assert_equal
