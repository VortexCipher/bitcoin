#!/usr/bin/env python3
# Copyright (c) 2017-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test the listsinceblock RPC."""

from test_framework.address import key_to_p2wpkh
from test_framework.blocktools import COINBASE_MATURITY
from test_framework.descriptors import descsum_create
from test_framework.test_framework import BitcoinTestFramework
from test_framework.messages import MAX_BIP125_RBF_SEQUENCE
from test_framework.util import (
    assert_array_result,
    assert_equal,
    assert_raises_rpc_error,
)
from test_framework.wallet_util import generate_keypair

from decimal import Decimal

class ListSinceBlockTest(BitcoinTestFramework):
    def add_options(self, parser):
        self.add_wallet_options(parser)

    def set_test_params(self):
        self.num_nodes = 4
        self.setup_clean_chain = True
        # whitelist peers to speed up tx relay / mempool sync
        self.noban_tx_relay = True

    def skip_test_if_missing_module(self):
        self.skip_if_no_wallet()

    def run_test(self):
        # All nodes are in IBD from genesis, so they'll need the miner (node2) to be an outbound connection, or have
        # only one connection. (See fPreferredDownload in net_processing)
        self.connect_nodes(1, 2)
        self.generate(self.nodes[2], COINBASE_MATURITY + 1)

        self.test_no_blockhash()
        self.test_invalid_blockhash()
        self.test_reorg()
        self.test_cant_read_block()
        self.test_double_spend()
        self.test_double_send()
        self.double_spends_filtered()
        self.test_targetconfirmations()
        if self.options.descriptors:
            self.test_desc()
        self.test_send_to_self()
