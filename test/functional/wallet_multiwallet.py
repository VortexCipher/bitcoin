#!/usr/bin/env python3
# Copyright (c) 2017-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test multiwallet.

Verify that a bitcoind node can load multiple wallet files
"""
from decimal import Decimal
from threading import Thread
import os
import platform
import shutil
import stat
import time

from test_framework.authproxy import JSONRPCException
from test_framework.blocktools import COINBASE_MATURITY
from test_framework.test_framework import BitcoinTestFramework
from test_framework.test_node import ErrorMatch
from test_framework.util import (
    assert_equal,
    assert_raises_rpc_error,
    get_rpc_proxy,
)

got_loading_error = False


def test_load_unload(node, name):
    global got_loading_error
    while True:
        if got_loading_error:
            return
        try:
            node.loadwallet(name)
            node.unloadwallet(name)
        except JSONRPCException as e:
            if e.error['code'] == -4 and 'Wallet already loading' in e.error['message']:
                got_loading_error = True
                return

