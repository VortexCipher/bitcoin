#!/usr/bin/env python3
# Copyright (c) 2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

import random
import time
from enum import Enum

from test_framework.messages import MAGIC_BYTES
from test_framework.p2p import P2PInterface
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import random_bitflip
from test_framework.v2_p2p import (
    EncryptedP2PState,
    MAX_GARBAGE_LEN,
)

