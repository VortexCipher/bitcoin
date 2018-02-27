#!/usr/bin/env python3
# Copyright (c) 2021-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test addrman functionality"""

import os
import re

from test_framework.messages import ser_uint256, hash256, MAGIC_BYTES
from test_framework.netutil import ADDRMAN_NEW_BUCKET_COUNT, ADDRMAN_TRIED_BUCKET_COUNT, ADDRMAN_BUCKET_SIZE
from test_framework.test_framework import BitcoinTestFramework
from test_framework.test_node import ErrorMatch
from test_framework.util import assert_equal

def serialize_addrman(
