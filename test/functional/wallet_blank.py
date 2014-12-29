#!/usr/bin/env python3
# Copyright (c) 2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or https://www.opensource.org/licenses/mit-license.php.


from test_framework.test_framework import BitcoinTestFramework
from test_framework.address import (
    ADDRESS_BCRT1_UNSPENDABLE,
    ADDRESS_BCRT1_UNSPENDABLE_DESCRIPTOR,
)
from test_framework.util import (
    assert_equal,
)
from test_framework.wallet_util import generate_keypair

