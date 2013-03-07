#!/usr/bin/env python3
# Copyright (c) 2023-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test for assumeutxo wallet related behavior.
See feature_assumeutxo.py for background.

## Possible test improvements

- TODO: test import descriptors while background sync is in progress
- TODO: test loading a wallet (backup) on a pruned node

"""
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
