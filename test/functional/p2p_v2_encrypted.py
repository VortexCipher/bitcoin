#!/usr/bin/env python3
# Copyright (c) 2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""
Test encrypted v2 p2p proposed in BIP 324
"""
from test_framework.blocktools import (
    create_block,
    create_coinbase,
)
from test_framework.p2p import (
    P2PDataStore,
