#!/usr/bin/env python3
# Copyright (c) 2014-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test the -alertnotify, -blocknotify and -walletnotify options."""
import os
import platform

from test_framework.address import ADDRESS_BCRT1_UNSPENDABLE
from test_framework.descriptors import descsum_create
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
)

# Linux allow all characters other than \x00
# Windows disallow control characters (0-31) and /\?%:|"<>
FILE_CHAR_START = 32 if platform.system() == 'Windows' else 1
FILE_CHAR_END = 128
