#!/usr/bin/env python3
# Copyright (c) 2015-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test p2p permission message.

Test that permissions are correctly calculated and applied
"""

from test_framework.messages import (
    SEQUENCE_FINAL,
)
from test_framework.p2p import P2PDataStore
from test_framework.test_node import ErrorMatch
from test_framework.test_framework import BitcoinTestFramework
