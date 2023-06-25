#!/usr/bin/env python3
# Copyright (c) 2024 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
from decimal import Decimal

from test_framework.messages import (
    MAX_BIP125_RBF_SEQUENCE,
    WITNESS_SCALE_FACTOR,
)
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    assert_greater_than,
    assert_greater_than_or_equal,
    assert_raises_rpc_error,
)
from test_framework.wallet import (
    COIN,
    DEFAULT_FEE,
    MiniWallet,
)

MAX_REPLACEMENT_CANDIDATES = 100
TRUC_MAX_VSIZE = 10000

def cleanup(extra_args=None):
    def decorator(func):
        def wrapper(self):
            try:
                if extra_args is not None:
                    self.restart_node(0, extra_args=extra_args)
                func(self)
            finally:
                # Clear mempool again after test
                self.generate(self.nodes[0], 1)
                if extra_args is not None:
                    self.restart_node(0)
        return wrapper
    return decorator

class MempoolTRUC(BitcoinTestFramework):
    def set_test_params(self):
        self.num_nodes = 1
        self.extra_args = [[]]
        self.setup_clean_chain = True

    def check_mempool(self, txids):
        """Assert exact contents of the node's mempool (by txid)."""
        mempool_contents = self.nodes[0].getrawmempool()
        assert_equal(len(txids), len(mempool_contents))
        assert all([txid in txids for txid in mempool_contents])

    @cleanup(extra_args=["-datacarriersize=20000"])
    def test_truc_max_vsize(self):
        node = self.nodes[0]
        self.log.info("Test TRUC-specific maximum transaction vsize")
        tx_v3_heavy = self.wallet.create_self_transfer(target_weight=(TRUC_MAX_VSIZE + 1) * WITNESS_SCALE_FACTOR, version=3)
        assert_greater_than_or_equal(tx_v3_heavy["tx"].get_vsize(), TRUC_MAX_VSIZE)
        expected_error_heavy = f"TRUC-violation, version=3 tx {tx_v3_heavy['txid']} (wtxid={tx_v3_heavy['wtxid']}) is too big"
        assert_raises_rpc_error(-26, expected_error_heavy, node.sendrawtransaction, tx_v3_heavy["hex"])
        self.check_mempool([])

        # Ensure we are hitting the TRUC-specific limit and not something else
