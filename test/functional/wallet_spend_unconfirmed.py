#!/usr/bin/env python3
# Copyright (c) 2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

from decimal import Decimal, getcontext

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_greater_than_or_equal,
    assert_equal,
    find_vout_for_address,
)

class UnconfirmedInputTest(BitcoinTestFramework):
    def add_options(self, parser):
        self.add_wallet_options(parser)

    def set_test_params(self):
        getcontext().prec=9
        self.setup_clean_chain = True
        self.num_nodes = 1

    def setup_and_fund_wallet(self, walletname):
        self.nodes[0].createwallet(walletname)
        wallet = self.nodes[0].get_wallet_rpc(walletname)

        self.def_wallet.sendtoaddress(address=wallet.getnewaddress(), amount=2)
        self.generate(self.nodes[0], 1) # confirm funding tx
        return wallet

    def skip_test_if_missing_module(self):
        self.skip_if_no_wallet()

    def calc_fee_rate(self, tx):
        fee = Decimal(-1e8) * tx["fee"]
        vsize = tx["decoded"]["vsize"]
        return fee / vsize

    def calc_set_fee_rate(self, txs):
        fees = Decimal(-1e8) * sum([tx["fee"] for tx in txs]) # fee is negative!
        vsizes = sum([tx["decoded"]["vsize"] for tx in txs])
        return fees / vsizes

    def assert_spends_only_parents(self, tx, parent_txids):
        parent_checklist = parent_txids.copy()
        number_inputs = len(tx["decoded"]["vin"])
        assert_equal(number_inputs, len(parent_txids))
        for i in range(number_inputs):
            txid_of_input = tx["decoded"]["vin"][i]["txid"]
