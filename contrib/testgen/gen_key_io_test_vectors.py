#!/usr/bin/env python3
# Copyright (c) 2012-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
'''
Generate valid and invalid base58/bech32(m) address and private key test vectors.
'''

from itertools import islice
import os
import random
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), '../../test/functional'))

from test_framework.address import base58_to_byte, byte_to_base58, b58chars  # noqa: E402
from test_framework.script import OP_0, OP_1, OP_2, OP_3, OP_16, OP_DUP, OP_EQUAL, OP_EQUALVERIFY, OP_HASH160, OP_CHECKSIG  # noqa: E402
from test_framework.segwit_addr import bech32_encode, decode_segwit_address, convertbits, CHARSET, Encoding  # noqa: E402

# key types
PUBKEY_ADDRESS = 0
SCRIPT_ADDRESS = 5
PUBKEY_ADDRESS_TEST = 111
SCRIPT_ADDRESS_TEST = 196
PUBKEY_ADDRESS_REGTEST = 111
SCRIPT_ADDRESS_REGTEST = 196
