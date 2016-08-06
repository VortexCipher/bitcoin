#!/usr/bin/env python3
# Copyright (c) 2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test initial headers download

Test that we only try to initially sync headers from one peer (until our chain
is close to caught up), and that each block announcement results in only one
additional peer receiving a getheaders message.
"""
