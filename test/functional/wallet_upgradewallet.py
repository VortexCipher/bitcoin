#!/usr/bin/env python3
# Copyright (c) 2018-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""upgradewallet RPC functional test

Test upgradewallet RPC. Download node binaries:

Requires previous releases binaries, see test/README.md.
Only v0.15.2 and v0.16.3 are required by this test.
"""

import os
import shutil
import struct

from io import BytesIO

from test_framework.blocktools import COINBASE_MATURITY
from test_framework.bdb import dump_bdb_kv
from test_framework.messages import deser_compact_size, deser_string
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    assert_is_hex_string,
    sha256sum_file,
)


UPGRADED_KEYMETA_VERSION = 12

def deser_keymeta(f):
    ver, create_time = struct.unpack('<Iq', f.read(12))
    kp_str = deser_string(f)
    seed_id = f.read(20)
    fpr = f.read(4)
