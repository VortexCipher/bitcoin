#!/usr/bin/env python3
# Copyright (c) 2020-2021 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""
Test BIP 37
"""

from test_framework.messages import (
    CInv,
    COIN,
    MAX_BLOOM_FILTER_SIZE,
    MAX_BLOOM_HASH_FUNCS,
    MSG_WTX,
    MSG_BLOCK,
    MSG_FILTERED_BLOCK,
    msg_filteradd,
    msg_filterclear,
    msg_filterload,
    msg_getdata,
    msg_mempool,
    msg_version,
)
from test_framework.p2p import (
    P2PInterface,
