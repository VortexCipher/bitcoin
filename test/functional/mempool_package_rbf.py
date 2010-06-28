#!/usr/bin/env python3
# Copyright (c) 2021 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

from decimal import Decimal

from test_framework.messages import (
    COIN,
    MAX_BIP125_RBF_SEQUENCE,
)
from test_framework.test_framework import BitcoinTestFramework
from test_framework.mempool_util import fill_mempool
from test_framework.util import (
    assert_greater_than_or_equal,
    assert_equal,
)
from test_framework.wallet import (
    DEFAULT_FEE,
    MiniWallet,
)

MAX_REPLACEMENT_CANDIDATES = 100

# Value high enough to cause evictions in each subtest
# for typical cases
DEFAULT_CHILD_FEE = DEFAULT_FEE * 4

class PackageRBFTest(BitcoinTestFramework):
    def set_test_params(self):
        self.num_nodes = 2
        self.setup_clean_chain = True
        # Required for fill_mempool()
        self.extra_args = [[
            "-datacarriersize=100000",
            "-maxmempool=5",
        ]] * self.num_nodes

    def assert_mempool_contents(self, expected=None):
        """Assert that all transactions in expected are in the mempool,
        and no additional ones exist.
        """
        if not expected:
            expected = []
        mempool = self.nodes[0].getrawmempool(verbose=False)
        assert_equal(len(mempool), len(expected))
        for tx in expected:
            assert tx.rehash() in mempool

    def create_simple_package(self, parent_coin, parent_fee=DEFAULT_FEE, child_fee=DEFAULT_CHILD_FEE, heavy_child=False):
        """Create a 1 parent 1 child package using the coin passed in as the parent's input. The
        parent has 1 output, used to fund 1 child transaction.
        All transactions signal BIP125 replaceability, but nSequence changes based on self.ctr. This
        prevents identical txids between packages when the parents spend the same coin and have the
        same fee (i.e. 0sat).

        returns tuple (hex serialized txns, CTransaction objects)
        """
        self.ctr += 1
