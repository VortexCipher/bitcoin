#!/usr/bin/env python3
# Copyright (c) 2018-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test the behavior of RPC importprivkey on set and unset labels of
addresses.

It tests different cases in which an address is imported with importaddress
with or without a label and then its private key is imported with importprivkey
with and without a label.
"""

