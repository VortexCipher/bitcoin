#!/usr/bin/env python3
# Copyright (c) 2017-2021 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test various fingerprinting protections.

If a stale block more than a month old or its header are requested by a peer,
the node should pretend that it does not have it to avoid fingerprinting.
"""

import time

from test_framework.blocktools import (create_block, create_coinbase)
