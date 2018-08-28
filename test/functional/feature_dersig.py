#!/usr/bin/env python3
# Copyright (c) 2015-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test BIP66 (DER SIG).

Test the DERSIG soft-fork activation on regtest.
"""

from test_framework.blocktools import (
    create_block,
    create_coinbase,
)
from test_framework.messages import msg_block
from test_framework.p2p import P2PInterface
