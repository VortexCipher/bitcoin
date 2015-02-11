#!/usr/bin/env python3
# Copyright (c) 2019-2021 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

""" Test node outbound peer eviction logic

A subset of our outbound peers are subject to eviction logic if they cannot keep up
with our vision of the best chain. This criteria applies only to non-protected peers,
and can be triggered by either not learning about any blocks from an outbound peer after
a certain deadline, or by them not being able to catch up fast enough (under the same deadline).

This tests the different eviction paths based on the peer's behavior and on whether they are protected
or not.
"""
import time

from test_framework.messages import (
    from_hex,
    msg_headers,
    CBlockHeader,
)
from test_framework.p2p import P2PInterface
from test_framework.test_framework import BitcoinTestFramework

