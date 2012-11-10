#!/usr/bin/env python3
# Copyright (c) 2017-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test that mempool.dat is both backward and forward compatible between versions

NOTE: The test is designed to prevent cases when compatibility is broken accidentally.
In case we need to break mempool compatibility we can continue to use the test by just bumping the version number.
