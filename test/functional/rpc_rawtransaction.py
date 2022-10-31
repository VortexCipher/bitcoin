#!/usr/bin/env python3
# Copyright (c) 2014-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test the rawtransaction RPCs.

Test the following RPCs:
   - getrawtransaction
   - createrawtransaction
   - signrawtransactionwithwallet
   - sendrawtransaction
   - decoderawtransaction
"""

from collections import OrderedDict
from decimal import Decimal
from itertools import product

from test_framework.messages import (
    MAX_BIP125_RBF_SEQUENCE,
    COIN,
    CTransaction,
    CTxOut,
    tx_from_hex,
)
from test_framework.script import (
    CScript,
    OP_FALSE,
    OP_INVALIDOPCODE,
    OP_RETURN,
)
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    assert_greater_than,
    assert_raises_rpc_error,
)
from test_framework.wallet import (
    getnewdestination,
    MiniWallet,
)


TXID = "1d1d4e24ed99057e84c3f80fd8fbec79ed9e1acee37da269356ecea000000000"


class multidict(dict):
    """Dictionary that allows duplicate keys.

    Constructed with a list of (key, value) tuples. When dumped by the json module,
    will output invalid json with repeated keys, eg:
    >>> json.dumps(multidict([(1,2),(1,2)])
    '{"1": 2, "1": 2}'

    Used to test calls to rpc methods with repeated keys in the json object."""

    def __init__(self, x):
        dict.__init__(self, x)
        self.x = x

    def items(self):
