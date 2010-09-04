#!/usr/bin/env python3
# Copyright (c) 2020-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test the send RPC command."""

from decimal import Decimal, getcontext
from itertools import product

from test_framework.authproxy import JSONRPCException
from test_framework.descriptors import descsum_create
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    assert_fee_amount,
    assert_greater_than,
    assert_raises_rpc_error,
    count_bytes,
)
from test_framework.wallet_util import (
    calculate_input_weight,
    generate_keypair,
)


class WalletSendTest(BitcoinTestFramework):
    def add_options(self, parser):
        self.add_wallet_options(parser)

    def set_test_params(self):
        self.num_nodes = 2
        # whitelist peers to speed up tx relay / mempool sync
        self.noban_tx_relay = True
        self.extra_args = [
            ["-walletrbf=1"],
            ["-walletrbf=1"]
        ]
        getcontext().prec = 8 # Satoshi precision for Decimal

    def skip_test_if_missing_module(self):
        self.skip_if_no_wallet()

    def test_send(self, from_wallet, to_wallet=None, amount=None, data=None,
                  arg_conf_target=None, arg_estimate_mode=None, arg_fee_rate=None,
                  conf_target=None, estimate_mode=None, fee_rate=None, add_to_wallet=None, psbt=None,
                  inputs=None, add_inputs=None, include_unsafe=None, change_address=None, change_position=None, change_type=None,
                  include_watching=None, locktime=None, lock_unspents=None, replaceable=None, subtract_fee_from_outputs=None,
                  expect_error=None, solving_data=None, minconf=None):
        assert (amount is None) != (data is None)

        from_balance_before = from_wallet.getbalances()["mine"]["trusted"]
        if include_unsafe:
            from_balance_before += from_wallet.getbalances()["mine"]["untrusted_pending"]

        if to_wallet is None:
            assert amount is None
        else:
            to_untrusted_pending_before = to_wallet.getbalances()["mine"]["untrusted_pending"]

        if amount:
            dest = to_wallet.getnewaddress()
