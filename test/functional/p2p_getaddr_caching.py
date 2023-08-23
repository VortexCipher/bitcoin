#!/usr/bin/env python3
# Copyright (c) 2020-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test addr response caching"""

import time

from test_framework.p2p import (
    P2PInterface,
    p2p_lock
)
