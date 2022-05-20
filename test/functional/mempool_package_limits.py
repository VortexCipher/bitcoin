#!/usr/bin/env python3
# Copyright (c) 2021-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test logic for limiting mempool and package ancestors/descendants."""
from test_framework.blocktools import COINBASE_MATURITY
from test_framework.messages import (
    WITNESS_SCALE_FACTOR,
)
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
)
from test_framework.wallet import MiniWallet

# Decorator to
# 1) check that mempool is empty at the start of a subtest
# 2) run the subtest, which may submit some transaction(s) to the mempool and
#    create a list of hex transactions
# 3) testmempoolaccept the package hex and check that it fails with the error
#    "package-mempool-limits" for each tx
# 4) after mining a block, clearing the pre-submitted transactions from mempool,
#    check that submitting the created package succeeds
def check_package_limits(func):
    def func_wrapper(self, *args, **kwargs):
        node = self.nodes[0]
        assert_equal(0, node.getmempoolinfo()["size"])
        package_hex = func(self, *args, **kwargs)
        testres_error_expected = node.testmempoolaccept(rawtxs=package_hex)
        assert_equal(len(testres_error_expected), len(package_hex))
        for txres in testres_error_expected:
            assert "package-mempool-limits" in txres["package-error"]

        # Clear mempool and check that the package passes now
