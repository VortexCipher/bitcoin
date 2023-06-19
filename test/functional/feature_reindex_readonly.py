#!/usr/bin/env python3
# Copyright (c) 2023-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test running bitcoind with -reindex from a read-only blockstore
- Start a node, generate blocks, then restart with -reindex after setting blk files to read-only
"""

import os
