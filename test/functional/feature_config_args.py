#!/usr/bin/env python3
# Copyright (c) 2017-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test various command line arguments and configuration file parameters."""

import os
from pathlib import Path
import platform
import re
import tempfile
import time

from test_framework.test_framework import BitcoinTestFramework
from test_framework.test_node import ErrorMatch
from test_framework import util


class ConfArgsTest(BitcoinTestFramework):
    def add_options(self, parser):
        self.add_wallet_options(parser)

    def set_test_params(self):
        self.setup_clean_chain = True
        self.num_nodes = 1
        self.supports_cli = False
        self.wallet_names = []
        self.disable_autoconnect = False

    def test_config_file_parser(self):
        self.log.info('Test config file parser')
        self.stop_node(0)

        # Check that startup fails if conf= is set in bitcoin.conf or in an included conf file
        bad_conf_file_path = self.nodes[0].datadir_path / "bitcoin_bad.conf"
        util.write_config(bad_conf_file_path, n=0, chain='', extra_config=f'conf=some.conf\n')
        conf_in_config_file_err = 'Error: Error reading configuration file: conf cannot be set in the configuration file; use includeconf= if you want to include additional config files'
        self.nodes[0].assert_start_raises_init_error(
            extra_args=[f'-conf={bad_conf_file_path}'],
            expected_msg=conf_in_config_file_err,
        )
        inc_conf_file_path = self.nodes[0].datadir_path / 'include.conf'
        with open(self.nodes[0].datadir_path / 'bitcoin.conf', 'a', encoding='utf-8') as conf:
            conf.write(f'includeconf={inc_conf_file_path}\n')
        with open(inc_conf_file_path, 'w', encoding='utf-8') as conf:
            conf.write('conf=some.conf\n')
