#!/usr/bin/env python3
# Copyright (c) 2014-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test mempool re-org scenarios.

Test re-org scenarios with a mempool that contains transactions
that spend (directly or indirectly) coinbase transactions.
"""

import time

from test_framework.messages import (
    CInv,
    MSG_WTX,
    msg_getdata,
)
from test_framework.p2p import (
    P2PTxInvStore,
