#!/usr/bin/env python3
# Copyright (c) 2017-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test RPC calls related to net.

Tests correspond to code in rpc/net.cpp.
"""

from decimal import Decimal
from itertools import product
import platform
import time

import test_framework.messages
from test_framework.p2p import (
    P2PInterface,
    P2P_SERVICES,
)
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_approx,
    assert_equal,
    assert_greater_than,
    assert_raises_rpc_error,
    p2p_port,
)
from test_framework.wallet import MiniWallet


def assert_net_servicesnames(servicesflag, servicenames):
    """Utility that checks if all flags are correctly decoded in
    `getpeerinfo` and `getnetworkinfo`.

    :param servicesflag: The services as an integer.
    :param servicenames: The list of decoded services names, as strings.
    """
    servicesflag_generated = 0
    for servicename in servicenames:
        servicesflag_generated |= getattr(test_framework.messages, 'NODE_' + servicename)
    assert servicesflag_generated == servicesflag


def seed_addrman(node):
    """ Populate the addrman with addresses from different networks.
    Here 2 ipv4, 2 ipv6, 1 cjdns, 2 onion and 1 i2p addresses are added.
    """
    # These addresses currently don't collide with a deterministic addrman.
    # If the addrman positioning/bucketing is changed, these might collide
    # and adding them fails.
    success = { "success": True }
    assert_equal(node.addpeeraddress(address="1.2.3.4", tried=True, port=8333), success)
    assert_equal(node.addpeeraddress(address="2.0.0.0", port=8333), success)
    assert_equal(node.addpeeraddress(address="1233:3432:2434:2343:3234:2345:6546:4534", tried=True, port=8333), success)
    assert_equal(node.addpeeraddress(address="2803:0:1234:abcd::1", port=45324), success)
    assert_equal(node.addpeeraddress(address="fc00:1:2:3:4:5:6:7", port=8333), success)
    assert_equal(node.addpeeraddress(address="pg6mmjiyjmcrsslvykfwnntlaru7p5svn6y2ymmju6nubxndf4pscryd.onion", tried=True, port=8333), success)
