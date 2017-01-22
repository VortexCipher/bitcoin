#!/usr/bin/env python3
# Copyright (c) 2021-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test a basic M-of-N multisig setup between multiple people using descriptor wallets and PSBTs, as well as a signing flow.

This is meant to be documentation as much as functional tests, so it is kept as simple and readable as possible.
"""

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_approx,
    assert_equal,
)

