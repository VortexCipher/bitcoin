#!/usr/bin/env python3
# Copyright (c) 2018-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test createwallet arguments.
"""

from test_framework.address import key_to_p2wpkh
from test_framework.descriptors import descsum_create
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    assert_raises_rpc_error,
)
from test_framework.wallet_util import generate_keypair, WalletUnlock


EMPTY_PASSPHRASE_MSG = "Empty string given as passphrase, wallet will not be encrypted."
LEGACY_WALLET_MSG = "Wallet created successfully. The legacy wallet type is being deprecated and support for creating and opening legacy wallets will be removed in the future."
