#!/usr/bin/env python3
# Copyright (c) 2020-2021 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test transaction relay behavior during IBD:
- Set fee filters to MAX_MONEY
- Don't request transactions
- Ignore all transaction messages
