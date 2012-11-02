#!/usr/bin/env python3
# Copyright (c) 2014-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test descendant package tracking carve-out allowing one final transaction in
   an otherwise-full package as long as it has only one parent and is <= 10k in
   size.
"""
