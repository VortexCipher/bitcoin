#!/usr/bin/env python3
# Copyright (c) The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

"""
Test that an attacker can't degrade compact block relay by sending unsolicited
mutated blocks to clear in-flight blocktxn requests from other honest peers.
"""

from test_framework.p2p import P2PInterface
