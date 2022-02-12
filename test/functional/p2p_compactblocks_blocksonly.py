#!/usr/bin/env python3
# Copyright (c) 2021-2021 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test that a node in blocksonly mode does not request compact blocks."""

from test_framework.messages import (
    MSG_BLOCK,
    MSG_CMPCT_BLOCK,
    MSG_WITNESS_FLAG,
    CBlock,
    CBlockHeader,
    CInv,
