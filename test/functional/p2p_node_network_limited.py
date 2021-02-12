#!/usr/bin/env python3
# Copyright (c) 2017-2021 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Tests NODE_NETWORK_LIMITED.

Tests that a node configured with -prune=550 signals NODE_NETWORK_LIMITED correctly
and that it responds to getdata requests for blocks correctly:
    - send a block within 288 + 2 of the tip
    - disconnect peers who request blocks older than that."""
from test_framework.messages import (
    CInv,
    MSG_BLOCK,
    NODE_NETWORK_LIMITED,
    NODE_P2P_V2,
    NODE_WITNESS,
    msg_getdata,
