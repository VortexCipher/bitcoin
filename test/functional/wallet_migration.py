#!/usr/bin/env python3
# Copyright (c) 2020-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test Migrating a wallet from legacy to descriptor."""

import random
import shutil
import struct
import time

from test_framework.address import (
    script_to_p2sh,
    key_to_p2pkh,
    key_to_p2wpkh,
)
from test_framework.bdb import BTREE_MAGIC
from test_framework.descriptors import descsum_create
from test_framework.key import ECPubKey
from test_framework.test_framework import BitcoinTestFramework
from test_framework.messages import COIN, CTransaction, CTxOut
from test_framework.script_util import key_to_p2pkh_script, script_to_p2sh_script, script_to_p2wsh_script
from test_framework.util import (
    assert_equal,
    assert_raises_rpc_error,
    sha256sum_file,
)
from test_framework.wallet_util import (
    get_generate_key,
)


class WalletMigrationTest(BitcoinTestFramework):
    def add_options(self, parser):
        self.add_wallet_options(parser)

    def set_test_params(self):
        self.setup_clean_chain = True
        self.num_nodes = 1
        self.extra_args = [[]]
        self.supports_cli = False

    def skip_test_if_missing_module(self):
        self.skip_if_no_wallet()
        self.skip_if_no_sqlite()
        self.skip_if_no_bdb()

    def assert_is_sqlite(self, wallet_name):
        wallet_file_path = self.nodes[0].wallets_path / wallet_name / self.wallet_data_filename
        with open(wallet_file_path, 'rb') as f:
            file_magic = f.read(16)
            assert_equal(file_magic, b'SQLite format 3\x00')
        assert_equal(self.nodes[0].get_wallet_rpc(wallet_name).getwalletinfo()["format"], "sqlite")

    def create_legacy_wallet(self, wallet_name, **kwargs):
        self.nodes[0].createwallet(wallet_name=wallet_name, descriptors=False, **kwargs)
        wallet = self.nodes[0].get_wallet_rpc(wallet_name)
        info = wallet.getwalletinfo()
        assert_equal(info["descriptors"], False)
        assert_equal(info["format"], "bdb")
        return wallet

    def migrate_wallet(self, wallet_rpc, *args, **kwargs):
        # Helper to ensure that only migration happens
        # Since we may rescan on loading of a wallet, make sure that the best block
        # is written before beginning migration
        # Reload to force write that record
        wallet_name = wallet_rpc.getwalletinfo()["walletname"]
        wallet_rpc.unloadwallet()
        self.nodes[0].loadwallet(wallet_name)
        # Migrate, checking that rescan does not occur
        with self.nodes[0].assert_debug_log(expected_msgs=[], unexpected_msgs=["Rescanning"]):
            return wallet_rpc.migratewallet(*args, **kwargs)

    def assert_addr_info_equal(self, addr_info, addr_info_old):
        assert_equal(addr_info["address"], addr_info_old["address"])
        assert_equal(addr_info["scriptPubKey"], addr_info_old["scriptPubKey"])
        assert_equal(addr_info["ismine"], addr_info_old["ismine"])
        assert_equal(addr_info["hdkeypath"], addr_info_old["hdkeypath"].replace("'","h"))
        assert_equal(addr_info["solvable"], addr_info_old["solvable"])
        assert_equal(addr_info["ischange"], addr_info_old["ischange"])
        assert_equal(addr_info["hdmasterfingerprint"], addr_info_old["hdmasterfingerprint"])

    def assert_list_txs_equal(self, received_list_txs, expected_list_txs):
        for d in received_list_txs:
            if "parent_descs" in d:
                del d["parent_descs"]
        for d in expected_list_txs:
            if "parent_descs" in d:
                del d["parent_descs"]
        assert_equal(received_list_txs, expected_list_txs)

    def check_address(self, wallet, addr, is_mine, is_change, label):
        addr_info = wallet.getaddressinfo(addr)
        assert_equal(addr_info['ismine'], is_mine)
        assert_equal(addr_info['ischange'], is_change)
        if label is not None:
            assert_equal(addr_info['labels'], [label]),
        else:
            assert_equal(addr_info['labels'], []),

    def test_basic(self):
        default = self.nodes[0].get_wallet_rpc(self.default_wallet_name)

