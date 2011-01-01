#!/usr/bin/env python3
# Copyright (c) 2014-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test descendant package tracking code."""

from decimal import Decimal

from test_framework.messages import (
    DEFAULT_ANCESTOR_LIMIT,
    DEFAULT_DESCENDANT_LIMIT,
)
from test_framework.p2p import P2PTxInvStore
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    assert_raises_rpc_error,
)
from test_framework.wallet import MiniWallet

# custom limits for node1
CUSTOM_ANCESTOR_LIMIT = 5
CUSTOM_DESCENDANT_LIMIT = 10
assert CUSTOM_DESCENDANT_LIMIT >= CUSTOM_ANCESTOR_LIMIT


class MempoolPackagesTest(BitcoinTestFramework):
    def set_test_params(self):
        self.num_nodes = 2
        # whitelist peers to speed up tx relay / mempool sync
