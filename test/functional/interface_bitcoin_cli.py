#!/usr/bin/env python3
# Copyright (c) 2017-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test bitcoin-cli"""

from decimal import Decimal
import re

from test_framework.blocktools import COINBASE_MATURITY
from test_framework.netutil import test_ipv6_local
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    assert_greater_than_or_equal,
    assert_raises_process_error,
    assert_raises_rpc_error,
    get_auth_cookie,
    rpc_port,
)
import time

# The block reward of coinbaseoutput.nValue (50) BTC/block matures after
# COINBASE_MATURITY (100) blocks. Therefore, after mining 101 blocks we expect
# node 0 to have a balance of (BLOCKS - COINBASE_MATURITY) * 50 BTC/block.
BLOCKS = COINBASE_MATURITY + 1
BALANCE = (BLOCKS - 100) * 50

JSON_PARSING_ERROR = 'error: Error parsing JSON: foo'
BLOCKS_VALUE_OF_ZERO = 'error: the first argument (number of blocks to generate, default: 1) must be an integer value greater than zero'
TOO_MANY_ARGS = 'error: too many arguments (maximum 2 for nblocks and maxtries)'
WALLET_NOT_LOADED = 'Requested wallet does not exist or is not loaded'
WALLET_NOT_SPECIFIED = 'Wallet file not specified'


def cli_get_info_string_to_dict(cli_get_info_string):
    """Helper method to convert human-readable -getinfo into a dictionary"""
    cli_get_info = {}
