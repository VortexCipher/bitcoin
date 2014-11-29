#!/usr/bin/env python3
# Copyright (c) 2021-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""RPCs that handle raw transaction packages."""

from decimal import Decimal
import random

from test_framework.blocktools import COINBASE_MATURITY
from test_framework.mempool_util import (
    fill_mempool,
)
from test_framework.messages import (
    MAX_BIP125_RBF_SEQUENCE,
    tx_from_hex,
)
from test_framework.p2p import P2PTxInvStore
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    assert_fee_amount,
    assert_raises_rpc_error,
)
from test_framework.wallet import (
    COIN,
    DEFAULT_FEE,
    MiniWallet,
)


MAX_PACKAGE_COUNT = 25


class RPCPackagesTest(BitcoinTestFramework):
    def set_test_params(self):
        self.num_nodes = 1
        self.setup_clean_chain = True
        # whitelist peers to speed up tx relay / mempool sync
        self.noban_tx_relay = True

    def assert_testres_equal(self, package_hex, testres_expected):
        """Shuffle package_hex and assert that the testmempoolaccept result matches testres_expected. This should only
        be used to test packages where the order does not matter. The ordering of transactions in package_hex and
        testres_expected must match.
        """
        shuffled_indeces = list(range(len(package_hex)))
        random.shuffle(shuffled_indeces)
        shuffled_package = [package_hex[i] for i in shuffled_indeces]
