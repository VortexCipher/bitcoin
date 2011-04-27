#!/usr/bin/env python3
# Copyright (c) 2024-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""
Test that 1p1c package submission allows a 1p1c package to propagate in a "network" of nodes. Send
various packages from different nodes on a network in which some nodes have already received some of
the transactions (and submitted them to mempool, kept them as orphans or rejected them as
too-low-feerate transactions). The packages should be received and accepted by all nodes.
"""

from decimal import Decimal
from math import ceil

from test_framework.mempool_util import (
    fill_mempool,
