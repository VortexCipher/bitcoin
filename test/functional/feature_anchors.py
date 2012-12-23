#!/usr/bin/env python3
# Copyright (c) 2020-2021 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test block-relay-only anchors functionality"""

import os

from test_framework.p2p import P2PInterface, P2P_SERVICES
from test_framework.socks5 import Socks5Configuration, Socks5Server
from test_framework.messages import CAddress, hash256
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import check_node_connections, assert_equal, p2p_port

