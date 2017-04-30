#!/usr/bin/env python3
# Copyright (c) 2020-2021 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""
Test addr relay
"""

import random
import time

from test_framework.messages import (
    CAddress,
    msg_addr,
    msg_getaddr,
    msg_verack,
)
from test_framework.p2p import (
    P2PInterface,
    p2p_lock,
    P2P_SERVICES,
)
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    assert_greater_than,
    assert_greater_than_or_equal
)

ONE_MINUTE  = 60
TEN_MINUTES = 10 * ONE_MINUTE
ONE_HOUR    = 60 * ONE_MINUTE
TWO_HOURS   =  2 * ONE_HOUR
ONE_DAY     = 24 * ONE_HOUR

ADDR_DESTINATIONS_THRESHOLD = 4

class AddrReceiver(P2PInterface):
    num_ipv4_received = 0
    test_addr_contents = False
    _tokens = 1
    send_getaddr = True

    def __init__(self, test_addr_contents=False, send_getaddr=True):
