#!/usr/bin/env python3
# Copyright (c) 2014-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test the importprunedfunds and removeprunedfunds RPCs."""
from decimal import Decimal

from test_framework.address import key_to_p2wpkh
from test_framework.blocktools import COINBASE_MATURITY
from test_framework.messages import (
    CMerkleBlock,
    from_hex,
)
from test_framework.test_framework import BitcoinTestFramework
