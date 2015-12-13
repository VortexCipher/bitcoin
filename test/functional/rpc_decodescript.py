#!/usr/bin/env python3
# Copyright (c) 2015-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test decoding scripts via decodescript RPC command."""

import json
import os

from test_framework.messages import (
    sha256,
    tx_from_hex,
)
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
)


class DecodeScriptTest(BitcoinTestFramework):
    def set_test_params(self):
        self.setup_clean_chain = True
        self.num_nodes = 1

    def decodescript_script_sig(self):
        signature = '304502207fa7a6d1e0ee81132a269ad84e68d695483745cde8b541e3bf630749894e342a022100c1f7ab20e13e22fb95281a870f3dcf38d782e53023ee313d741ad0cfbc0c509001'
        push_signature = '48' + signature
        public_key = '03b0da749730dc9b4b1f4a14d6902877a92541f5368778853d9c4a0cb7802dcfb2'
        push_public_key = '21' + public_key

