#!/usr/bin/env python3
# Copyright (c) 2018-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test bitcoin-wallet."""

import os
import platform
import stat
import subprocess
import textwrap

from collections import OrderedDict

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    assert_greater_than,
    sha256sum_file,
)


class ToolWalletTest(BitcoinTestFramework):
    def add_options(self, parser):
        self.add_wallet_options(parser)
        parser.add_argument("--bdbro", action="store_true", help="Use the BerkeleyRO internal parser when dumping a Berkeley DB wallet file")
        parser.add_argument("--swap-bdb-endian", action="store_true",help="When making Legacy BDB wallets, always make then byte swapped internally")

    def set_test_params(self):
        self.num_nodes = 1
        self.setup_clean_chain = True
        self.rpc_timeout = 120
        if self.options.swap_bdb_endian:
            self.extra_args = [["-swapbdbendian"]]

    def skip_test_if_missing_module(self):
        self.skip_if_no_wallet()
        self.skip_if_no_wallet_tool()

    def bitcoin_wallet_process(self, *args):
        default_args = ['-datadir={}'.format(self.nodes[0].datadir_path), '-chain=%s' % self.chain]
        if not self.options.descriptors and 'create' in args:
            default_args.append('-legacy')
        if "dump" in args and self.options.bdbro:
            default_args.append("-withinternalbdb")

        return subprocess.Popen([self.options.bitcoinwallet] + default_args + list(args), stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    def assert_raises_tool_error(self, error, *args):
        p = self.bitcoin_wallet_process(*args)
        stdout, stderr = p.communicate()
        assert_equal(stdout, '')
        if isinstance(error, tuple):
            assert_equal(p.poll(), error[0])
            assert error[1] in stderr.strip()
        else:
