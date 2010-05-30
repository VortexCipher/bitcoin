#!/usr/bin/env python3
# Copyright (c) 2017-2021 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test transaction upload"""

from test_framework.messages import msg_getdata, CInv, MSG_TX, MSG_WTX
from test_framework.p2p import p2p_lock, P2PDataStore, P2PTxInvStore
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
