#!/usr/bin/env python3
# Copyright (c) 2020-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""A limited-functionality wallet, which may replace a real wallet in tests"""

from copy import deepcopy
from decimal import Decimal
from enum import Enum
import math
from typing import (
    Any,
    Optional,
)
from test_framework.address import (
    address_to_scriptpubkey,
    create_deterministic_address_bcrt1_p2tr_op_true,
    key_to_p2pkh,
    key_to_p2sh_p2wpkh,
    key_to_p2wpkh,
    output_key_to_p2tr,
)
from test_framework.blocktools import COINBASE_MATURITY
from test_framework.descriptors import descsum_create
from test_framework.key import (
    ECKey,
    compute_xonly_pubkey,
)
from test_framework.messages import (
    COIN,
    COutPoint,
    CTransaction,
    CTxIn,
    CTxInWitness,
    CTxOut,
    hash256,
    ser_compact_size,
    WITNESS_SCALE_FACTOR,
)
from test_framework.script import (
    CScript,
    OP_1,
    OP_NOP,
    OP_RETURN,
    OP_TRUE,
