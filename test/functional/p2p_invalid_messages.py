#!/usr/bin/env python3
# Copyright (c) 2015-2021 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test node responses to invalid network messages."""

import random
import time

from test_framework.messages import (
    CBlockHeader,
    CInv,
    MAX_HEADERS_RESULTS,
    MAX_INV_SIZE,
    MAX_PROTOCOL_MESSAGE_LENGTH,
    MSG_TX,
    from_hex,
    msg_getdata,
    msg_headers,
    msg_inv,
    msg_ping,
    msg_version,
    ser_string,
)
from test_framework.p2p import (
    P2PDataStore,
    P2PInterface,
)
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
)

VALID_DATA_LIMIT = MAX_PROTOCOL_MESSAGE_LENGTH - 5  # Account for the 5-byte length prefix

