#!/usr/bin/env python3
# Copyright (c) 2017-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test that the mempool ensures transaction delivery by periodically sending
to peers until a GETDATA is received."""

import time

from test_framework.p2p import P2PTxInvStore
from test_framework.test_framework import BitcoinTestFramework
