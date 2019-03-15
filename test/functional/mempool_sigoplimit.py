#!/usr/bin/env python3
# Copyright (c) 2023 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test sigop limit mempool policy (`-bytespersigop` parameter)"""
from decimal import Decimal
from math import ceil

from test_framework.messages import (
    COutPoint,
    CTransaction,
    CTxIn,
    CTxInWitness,
    CTxOut,
    WITNESS_SCALE_FACTOR,
    tx_from_hex,
)
from test_framework.script import (
    CScript,
