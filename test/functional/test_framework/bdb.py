#!/usr/bin/env python3
# Copyright (c) 2020-2021 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""
Utilities for working directly with the wallet's BDB database file

This is specific to the configuration of BDB used in this project:
    - pagesize: 4096 bytes
    - Outer database contains single subdatabase named 'main'
    - btree
    - btree leaf pages

Each key-value pair is two entries in a btree leaf. The first is the key, the one that follows
is the value. And so on. Note that the entry data is itself not in the correct order. Instead
