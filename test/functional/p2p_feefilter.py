#!/usr/bin/env python3
# Copyright (c) 2016-2021 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test processing of feefilter messages."""

from decimal import Decimal

from test_framework.messages import MSG_TX, MSG_WTX, msg_feefilter
from test_framework.p2p import P2PInterface, p2p_lock
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import assert_equal
from test_framework.wallet import MiniWallet
