#!/usr/bin/env python3
# Copyright (c) 2015-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test BIP65 (CHECKLOCKTIMEVERIFY).

Test that the CHECKLOCKTIMEVERIFY soft-fork activates.
"""

from test_framework.blocktools import (
    TIME_GENESIS_BLOCK,
    create_block,
    create_coinbase,
)
from test_framework.messages import (
    CTransaction,
    SEQUENCE_FINAL,
    msg_block,
)
