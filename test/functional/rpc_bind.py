#!/usr/bin/env python3
# Copyright (c) 2014-2019 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test running bitcoind with the -rpcbind and -rpcallowip options."""

from test_framework.netutil import all_interfaces, addr_to_hex, get_bind_addrs, test_ipv6_local
from test_framework.test_framework import BitcoinTestFramework, SkipTest
from test_framework.util import assert_equal, assert_raises_rpc_error, get_rpc_proxy, rpc_port, rpc_url

class RPCBindTest(BitcoinTestFramework):
    def set_test_params(self):
