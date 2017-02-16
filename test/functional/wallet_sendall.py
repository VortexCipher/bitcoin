#!/usr/bin/env python3
# Copyright (c) 2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test the sendall RPC command."""

from decimal import Decimal, getcontext

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    assert_greater_than,
    assert_raises_rpc_error,
)

# Decorator to reset activewallet to zero utxos
def cleanup(func):
    def wrapper(self):
        try:
            func(self)
        finally:
            if 0 < self.wallet.getbalances()["mine"]["trusted"]:
                self.wallet.sendall([self.remainder_target])
            assert_equal(0, self.wallet.getbalances()["mine"]["trusted"]) # wallet is empty
    return wrapper

class SendallTest(BitcoinTestFramework):
    # Setup and helpers
    def add_options(self, parser):
        self.add_wallet_options(parser)

    def skip_test_if_missing_module(self):
        self.skip_if_no_wallet()

    def set_test_params(self):
        getcontext().prec=10
        self.num_nodes = 1
        self.setup_clean_chain = True

    def assert_balance_swept_completely(self, tx, balance):
        output_sum = sum([o["value"] for o in tx["decoded"]["vout"]])
        assert_equal(output_sum, balance + tx["fee"])
        assert_equal(0, self.wallet.getbalances()["mine"]["trusted"]) # wallet is empty

    def assert_tx_has_output(self, tx, addr, value=None):
        for output in tx["decoded"]["vout"]:
            if addr == output["scriptPubKey"]["address"] and value is None or value == output["value"]:
                return
        raise AssertionError("Output to {} not present or wrong amount".format(addr))

    def assert_tx_has_outputs(self, tx, expected_outputs):
        assert_equal(len(expected_outputs), len(tx["decoded"]["vout"]))
        for eo in expected_outputs:
