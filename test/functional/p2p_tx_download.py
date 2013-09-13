#!/usr/bin/env python3
# Copyright (c) 2019-2021 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""
Test transaction download behavior
"""
from decimal import Decimal
import time

from test_framework.mempool_util import (
    fill_mempool,
)
from test_framework.messages import (
    CInv,
    MSG_TX,
    MSG_TYPE_MASK,
    MSG_WTX,
    msg_inv,
    msg_notfound,
    msg_tx,
)
from test_framework.p2p import (
    P2PInterface,
    p2p_lock,
)
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
)
