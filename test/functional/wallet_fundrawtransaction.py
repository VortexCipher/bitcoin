#!/usr/bin/env python3
# Copyright (c) 2014-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test the fundrawtransaction RPC."""


from decimal import Decimal
from itertools import product
from math import ceil
from test_framework.address import address_to_scriptpubkey

from test_framework.descriptors import descsum_create
from test_framework.messages import (
    COIN,
    CTransaction,
    CTxOut,
)
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_approx,
    assert_equal,
    assert_fee_amount,
    assert_greater_than,
    assert_greater_than_or_equal,
    assert_raises_rpc_error,
    count_bytes,
    get_fee,
)
from test_framework.wallet_util import generate_keypair, WalletUnlock

ERR_NOT_ENOUGH_PRESET_INPUTS = "The preselected coins total amount does not cover the transaction target. " \
                               "Please allow other inputs to be automatically selected or include more coins manually"

def get_unspent(listunspent, amount):
    for utx in listunspent:
        if utx['amount'] == amount:
            return utx
    raise AssertionError('Could not find unspent with amount={}'.format(amount))

class RawTransactionsTest(BitcoinTestFramework):
    def add_options(self, parser):
        self.add_wallet_options(parser)

    def set_test_params(self):
        self.num_nodes = 4
        self.setup_clean_chain = True
        # whitelist peers to speed up tx relay / mempool sync
        self.noban_tx_relay = True
        self.rpc_timeout = 90  # to prevent timeouts in `test_transaction_too_large`

    def skip_test_if_missing_module(self):
        self.skip_if_no_wallet()

    def setup_network(self):
        self.setup_nodes()

        self.connect_nodes(0, 1)
        self.connect_nodes(1, 2)
        self.connect_nodes(0, 2)
        self.connect_nodes(0, 3)

    def lock_outputs_type(self, wallet, outputtype):
        """
        Only allow UTXOs of the given type
        """
        if outputtype in ["legacy", "p2pkh", "pkh"]:
            prefixes = ["pkh(", "sh(multi("]
        elif outputtype in ["p2sh-segwit", "sh_wpkh"]:
            prefixes = ["sh(wpkh(", "sh(wsh("]
        elif outputtype in ["bech32", "wpkh"]:
            prefixes = ["wpkh(", "wsh("]
        else:
            assert False, f"Unknown output type {outputtype}"

        to_lock = []
        for utxo in wallet.listunspent():
            if "desc" in utxo:
                for prefix in prefixes:
                    if utxo["desc"].startswith(prefix):
                        to_lock.append({"txid": utxo["txid"], "vout": utxo["vout"]})
        wallet.lockunspent(False, to_lock)

    def unlock_utxos(self, wallet):
        """
        Unlock all UTXOs except the watchonly one
        """
        to_keep = []
        if self.watchonly_utxo is not None:
            to_keep.append(self.watchonly_utxo)
        wallet.lockunspent(True)
        wallet.lockunspent(False, to_keep)

    def run_test(self):
        self.watchonly_utxo = None
        self.log.info("Connect nodes, set fees, generate blocks, and sync")
        self.min_relay_tx_fee = self.nodes[0].getnetworkinfo()['relayfee']
        # This test is not meant to test fee estimation and we'd like
        # to be sure all txs are sent at a consistent desired feerate
        for node in self.nodes:
            node.settxfee(self.min_relay_tx_fee)

        # if the fee's positive delta is higher than this value tests will fail,
        # neg. delta always fail the tests.
        # The size of the signature of every input may be at most 2 bytes larger
        # than a minimum sized signature.

        #            = 2 bytes * minRelayTxFeePerByte
        self.fee_tolerance = 2 * self.min_relay_tx_fee / 1000

        self.generate(self.nodes[2], 1)
        self.generate(self.nodes[0], 121)

        self.test_add_inputs_default_value()
        self.test_preset_inputs_selection()
        self.test_weight_calculation()
        self.test_weight_limits()
        self.test_change_position()
        self.test_simple()
        self.test_simple_two_coins()
        self.test_simple_two_outputs()
        self.test_change()
        self.test_no_change()
        self.test_invalid_option()
        self.test_invalid_change_address()
        self.test_valid_change_address()
        self.test_change_type()
        self.test_coin_selection()
        self.test_two_vin()
        self.test_two_vin_two_vout()
        self.test_invalid_input()
        self.test_fee_p2pkh()
        self.test_fee_p2pkh_multi_out()
        self.test_fee_p2sh()
        self.test_fee_4of5()
        self.test_spend_2of2()
        self.test_locked_wallet()
        self.test_many_inputs_fee()
        self.test_many_inputs_send()
        self.test_op_return()
        self.test_watchonly()
        self.test_all_watched_funds()
        self.test_option_feerate()
        self.test_address_reuse()
        self.test_option_subtract_fee_from_outputs()
        self.test_subtract_fee_with_presets()
        self.test_transaction_too_large()
        self.test_include_unsafe()
        self.test_external_inputs()
        self.test_22670()
        self.test_feerate_rounding()
        self.test_input_confs_control()
