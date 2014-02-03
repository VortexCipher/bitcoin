#!/usr/bin/env python3
# Copyright (c) 2010 ArtForz -- public domain half-a-node
# Copyright (c) 2012 Jeff Garzik
# Copyright (c) 2010-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test objects for interacting with a bitcoind node over the p2p protocol.

The P2PInterface objects interact with the bitcoind nodes under test using the
node's p2p interface. They can be used to send messages to the node, and
callbacks can be registered that execute when messages are received from the
node. Messages are sent to/received from the node on an asyncio event loop.
State held inside the objects must be guarded by the p2p_lock to avoid data
races between the main testing thread and the event loop.

P2PConnection: A low-level connection object to a node's P2P interface
P2PInterface: A high-level interface object for communicating to a node over P2P
P2PDataStore: A p2p interface class that keeps a store of transactions and blocks
              and can respond correctly to getdata and getheaders messages
P2PTxInvStore: A p2p interface class that inherits from P2PDataStore, and keeps
              a count of how many times each txid has been announced."""

import asyncio
from collections import defaultdict
from io import BytesIO
import logging
import platform
import struct
import sys
import threading

from test_framework.messages import (
    CBlockHeader,
    MAX_HEADERS_RESULTS,
    msg_addr,
    msg_addrv2,
    msg_block,
    MSG_BLOCK,
    msg_blocktxn,
    msg_cfcheckpt,
    msg_cfheaders,
    msg_cfilter,
    msg_cmpctblock,
    msg_feefilter,
    msg_filteradd,
    msg_filterclear,
    msg_filterload,
    msg_getaddr,
    msg_getblocks,
    msg_getblocktxn,
    msg_getcfcheckpt,
    msg_getcfheaders,
    msg_getcfilters,
    msg_getdata,
    msg_getheaders,
    msg_headers,
    msg_inv,
    msg_mempool,
    msg_merkleblock,
    msg_notfound,
    msg_ping,
    msg_pong,
    msg_sendaddrv2,
    msg_sendcmpct,
    msg_sendheaders,
    msg_sendtxrcncl,
    msg_tx,
    MSG_TX,
    MSG_TYPE_MASK,
    msg_verack,
    msg_version,
    MSG_WTX,
    msg_wtxidrelay,
    NODE_NETWORK,
    NODE_WITNESS,
    MAGIC_BYTES,
    sha256,
)
from test_framework.util import (
    MAX_NODES,
    p2p_port,
    wait_until_helper_internal,
)
from test_framework.v2_p2p import (
    EncryptedP2PState,
    MSGTYPE_TO_SHORTID,
    SHORTID,
)

logger = logging.getLogger("TestFramework.p2p")

# The minimum P2P version that this test framework supports
MIN_P2P_VERSION_SUPPORTED = 60001
# The P2P version that this test framework implements and sends in its `version` message
