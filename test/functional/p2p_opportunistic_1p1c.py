#!/usr/bin/env python3
# Copyright (c) 2024-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""
Test opportunistic 1p1c package submission logic.
"""

from decimal import Decimal
import time
from test_framework.mempool_util import (
    fill_mempool,
)
from test_framework.messages import (
    CInv,
    CTxInWitness,
    MAX_BIP125_RBF_SEQUENCE,
    MSG_WTX,
    msg_inv,
    msg_tx,
    tx_from_hex,
)
from test_framework.p2p import (
    P2PInterface,
)
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    assert_greater_than,
)
from test_framework.wallet import (
    MiniWallet,
    MiniWalletMode,
)

# 1sat/vB feerate denominated in BTC/KvB
FEERATE_1SAT_VB = Decimal("0.00001000")
# Number of seconds to wait to ensure no getdata is received
GETDATA_WAIT = 60

def cleanup(func):
