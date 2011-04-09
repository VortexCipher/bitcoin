#!/usr/bin/env python3
# Copyright (c) 2019-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test p2p blocksonly mode & block-relay-only connections."""

import time

from test_framework.messages import msg_tx, msg_inv, CInv, MSG_WTX
from test_framework.p2p import P2PInterface, P2PTxInvStore
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import assert_equal
