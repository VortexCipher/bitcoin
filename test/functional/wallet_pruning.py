#!/usr/bin/env python3
# Copyright (c) 2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

"""Test wallet import on pruned node."""

from test_framework.util import assert_equal, assert_raises_rpc_error
from test_framework.blocktools import (
    COINBASE_MATURITY,
    create_block
)
from test_framework.blocktools import create_coinbase
from test_framework.test_framework import BitcoinTestFramework

