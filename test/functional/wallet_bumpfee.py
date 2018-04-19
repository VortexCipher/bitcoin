#!/usr/bin/env python3
# Copyright (c) 2016-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test the bumpfee RPC.

Verifies that the bumpfee RPC creates replacement transactions successfully when
its preconditions are met, and returns appropriate errors in other cases.

This module consists of around a dozen individual test cases implemented in the
top-level functions named as test_<test_case_description>. The test functions
can be disabled or reordered if needed for debugging. If new test cases are
added in the future, they should try to follow the same convention and not
make assumptions about execution order.
"""
from decimal import Decimal

from test_framework.blocktools import (
    COINBASE_MATURITY,
)
from test_framework.messages import (
    MAX_BIP125_RBF_SEQUENCE,
)
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    assert_fee_amount,
    assert_greater_than,
    assert_raises_rpc_error,
    get_fee,
    find_vout_for_address,
)
from test_framework.wallet import MiniWallet


WALLET_PASSPHRASE = "test"
WALLET_PASSPHRASE_TIMEOUT = 3600

# Fee rates (sat/vB)
INSUFFICIENT =      1
ECONOMICAL   =     50
NORMAL       =    100
HIGH         =    500
TOO_HIGH     = 100000

def get_change_address(tx, node):
    tx_details = node.getrawtransaction(tx, 1)
    txout_addresses = [txout['scriptPubKey']['address'] for txout in tx_details["vout"]]
    return [address for address in txout_addresses if node.getaddressinfo(address)["ischange"]]

class BumpFeeTest(BitcoinTestFramework):
    def add_options(self, parser):
        self.add_wallet_options(parser)

    def set_test_params(self):
        self.num_nodes = 2
        self.setup_clean_chain = True
        # whitelist peers to speed up tx relay / mempool sync
        self.noban_tx_relay = True
        self.extra_args = [[
            "-walletrbf={}".format(i),
            "-mintxfee=0.00002",
            "-addresstype=bech32",
        ] for i in range(self.num_nodes)]

    def skip_test_if_missing_module(self):
        self.skip_if_no_wallet()

    def clear_mempool(self):
        # Clear mempool between subtests. The subtests may only depend on chainstate (utxos)
        self.generate(self.nodes[1], 1)

    def run_test(self):
        # Encrypt wallet for test_locked_wallet_fails test
        self.nodes[1].encryptwallet(WALLET_PASSPHRASE)
        self.nodes[1].walletpassphrase(WALLET_PASSPHRASE, WALLET_PASSPHRASE_TIMEOUT)

        peer_node, rbf_node = self.nodes
        rbf_node_address = rbf_node.getnewaddress()

        # fund rbf node with 10 coins of 0.001 btc (100,000 satoshis)
        self.log.info("Mining blocks...")
        self.generate(peer_node, 110)
        for _ in range(25):
            peer_node.sendtoaddress(rbf_node_address, 0.001)
