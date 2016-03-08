#!/usr/bin/env python3
# Copyright (c) 2014-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test BIP68 implementation."""

import time

from test_framework.blocktools import (
    NORMAL_GBT_REQUEST_PARAMS,
    add_witness_commitment,
    create_block,
    script_to_p2wsh_script,
)
from test_framework.messages import (
    COIN,
    COutPoint,
    CTransaction,
    CTxIn,
    CTxInWitness,
    CTxOut,
    tx_from_hex,
)
from test_framework.script import (
    CScript,
    OP_TRUE,
)
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    assert_greater_than,
    assert_raises_rpc_error,
    softfork_active,
)
from test_framework.wallet import MiniWallet

SCRIPT_W0_SH_OP_TRUE = script_to_p2wsh_script(CScript([OP_TRUE]))

SEQUENCE_LOCKTIME_DISABLE_FLAG = (1<<31)
SEQUENCE_LOCKTIME_TYPE_FLAG = (1<<22) # this means use time (0 means height)
SEQUENCE_LOCKTIME_GRANULARITY = 9 # this is a bit-shift
