#!/usr/bin/env python3
# Copyright (c) 2014-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test the REST API."""

from decimal import Decimal
from enum import Enum
import http.client
import json
import typing
import urllib.parse


from test_framework.messages import (
    BLOCK_HEADER_SIZE,
    COIN,
)
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    assert_greater_than,
    assert_greater_than_or_equal,
)
from test_framework.wallet import (
    MiniWallet,
    getnewdestination,
)
from typing import Optional


INVALID_PARAM = "abc"
UNKNOWN_PARAM = "0000000000000000000000000000000000000000000000000000000000000000"


class ReqType(Enum):
    JSON = 1
    BIN = 2
    HEX = 3

class RetType(Enum):
    OBJ = 1
    BYTES = 2
    JSON = 3
