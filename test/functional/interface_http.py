#!/usr/bin/env python3
# Copyright (c) 2014-2021 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test the RPC HTTP basics."""

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import assert_equal, str_to_b64str

import http.client
