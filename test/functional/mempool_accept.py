#!/usr/bin/env python3
# Copyright (c) 2017-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test mempool acceptance of raw transactions."""

from copy import deepcopy
from decimal import Decimal
import math

from test_framework.test_framework import BitcoinTestFramework
from test_framework.messages import (
    MAX_BIP125_RBF_SEQUENCE,
    COIN,
    COutPoint,
    CTransaction,
    CTxIn,
    CTxInWitness,
    CTxOut,
    MAX_BLOCK_WEIGHT,
    WITNESS_SCALE_FACTOR,
    MAX_MONEY,
    SEQUENCE_FINAL,
    tx_from_hex,
)
from test_framework.script import (
    CScript,
    OP_0,
    OP_HASH160,
    OP_RETURN,
    OP_TRUE,
    SIGHASH_ALL,
    sign_input_legacy,
)
from test_framework.script_util import (
    DUMMY_MIN_OP_RETURN_SCRIPT,
    keys_to_multisig_script,
    MIN_PADDING,
    MIN_STANDARD_TX_NONWITNESS_SIZE,
    PAY_TO_ANCHOR,
    script_to_p2sh_script,
    script_to_p2wsh_script,
)
from test_framework.util import (
    assert_equal,
    assert_greater_than,
    assert_raises_rpc_error,
