#!/usr/bin/env python3
# Copyright (c) 2020-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test the getblockfrompeer RPC."""

from test_framework.authproxy import JSONRPCException
from test_framework.messages import (
    CBlock,
    from_hex,
    msg_headers,
    NODE_WITNESS,
)
from test_framework.p2p import (
    P2P_SERVICES,
