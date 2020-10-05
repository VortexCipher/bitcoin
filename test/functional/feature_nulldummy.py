#!/usr/bin/env python3
# Copyright (c) 2016-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test NULLDUMMY softfork.

Connect to a single node.
Generate 2 blocks (save the coinbases for later).
Generate COINBASE_MATURITY (CB) more blocks to ensure the coinbases are mature.
[Policy/Consensus] Check that NULLDUMMY compliant transactions are accepted in block CB + 3.
[Policy] Check that non-NULLDUMMY transactions are rejected before activation.
[Consensus] Check that the new NULLDUMMY rules are not enforced on block CB + 4.
[Policy/Consensus] Check that the new NULLDUMMY rules are enforced on block CB + 5.
"""
import time
