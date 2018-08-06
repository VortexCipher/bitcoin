#!/usr/bin/env python3
# Copyright (c) 2014-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test RPCs related to blockchainstate.

Test the following RPCs:
    - getblockchaininfo
    - getdeploymentinfo
    - getchaintxstats
    - gettxoutsetinfo
    - getblockheader
    - getdifficulty
    - getnetworkhashps
    - waitforblockheight
    - getblock
    - getblockhash
    - getbestblockhash
    - verifychain

Tests correspond to code in rpc/blockchain.cpp.
"""

from decimal import Decimal
import http.client
import os
import subprocess
import textwrap

from test_framework.blocktools import (
    MAX_FUTURE_BLOCK_TIME,
    TIME_GENESIS_BLOCK,
    create_block,
    create_coinbase,
)
from test_framework.messages import (
    CBlockHeader,
    from_hex,
    msg_block,
)
from test_framework.p2p import P2PInterface
from test_framework.script import hash256
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    assert_greater_than,
    assert_greater_than_or_equal,
    assert_raises,
    assert_raises_rpc_error,
    assert_is_hex_string,
    assert_is_hash_string,
)
from test_framework.wallet import MiniWallet


HEIGHT = 200  # blocks mined
TIME_RANGE_STEP = 600  # ten-minute steps
TIME_RANGE_MTP = TIME_GENESIS_BLOCK + (HEIGHT - 6) * TIME_RANGE_STEP
TIME_RANGE_TIP = TIME_GENESIS_BLOCK + (HEIGHT - 1) * TIME_RANGE_STEP
TIME_RANGE_END = TIME_GENESIS_BLOCK + HEIGHT * TIME_RANGE_STEP
DIFFICULTY_ADJUSTMENT_INTERVAL = 2016


