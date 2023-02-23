#!/usr/bin/env python3
# Copyright (c) 2021-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test generation and spending of P2TR addresses."""

import random
import uuid

from decimal import Decimal
from test_framework.address import output_key_to_p2tr
from test_framework.key import H_POINT
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import assert_equal
from test_framework.descriptors import descsum_create
from test_framework.script import (
    CScript,
    MAX_PUBKEYS_PER_MULTI_A,
    OP_CHECKSIG,
    OP_CHECKSIGADD,
    OP_NUMEQUAL,
    taproot_construct,
)
from test_framework.segwit_addr import encode_segwit_address

# xprvs/xpubs, and m/* derived x-only pubkeys (created using independent implementation)
KEYS = [
    {
        "xprv": "tprv8ZgxMBicQKsPeNLUGrbv3b7qhUk1LQJZAGMuk9gVuKh9sd4BWGp1eMsehUni6qGb8bjkdwBxCbgNGdh2bYGACK5C5dRTaif9KBKGVnSezxV",
        "xpub": "tpubD6NzVbkrYhZ4XqNGAWGWSzmxGWFwVjVTjZxh2fioKbVYi7Jx8fdbprVWsdW7mHwqjchBVas8TLZG4Xwuz4RKU4iaCqiCvoSkFCzQptqk5Y1",
        "pubs": [
            "83d8ee77a0f3a32a5cea96fd1624d623b836c1e5d1ac2dcde46814b619320c18",
            "a30253b018ea6fca966135bf7dd8026915427f24ccf10d4e03f7870f4128569b",
            "a61e5749f2f3db9dc871d7b187e30bfd3297eea2557e9be99897ea8ff7a29a21",
            "8110cf482f66dc37125e619d73075af932521724ffc7108309e88f361efe8c8a",
        ]
    },
    {
        "xprv": "tprv8ZgxMBicQKsPe98QUPieXy5KFPVjuZNpcC9JY7K7buJEm8nWvJogK4kTda7eLjK9U4PnMNbSjEkpjDJazeBZ4rhYNYD7N6GEdaysj1AYSb5",
        "xpub": "tpubD6NzVbkrYhZ4XcACN3PEwNjRpR1g4tZjBVk5pdMR2B6dbd3HYhdGVZNKofAiFZd9okBserZvv58A6tBX4pE64UpXGNTSesfUW7PpW36HuKz",
        "pubs": [
            "f95886b02a84928c5c15bdca32784993105f73de27fa6ad8c1a60389b999267c",
            "71522134160685eb779857033bfc84c7626f13556154653a51dd42619064e679",
            "48957b4158b2c5c3f4c000f51fd2cf0fd5ff8868ebfb194256f5e9131fc74bd8",
            "086dda8139b3a84944010648d2b674b70447be3ae59322c09a4907bc80be62c1",
        ]
    },
    {
        "xprv": "tprv8ZgxMBicQKsPe3ZJmcj9aJ2EPZJYYCh6Lp3v82p75wspgaXmtDZ2RBtkAtWcGnW2VQDzMHQPBkCKMoYTqh1RfJKjv4PcmWVR7KqTpjsdboN",
        "xpub": "tpubD6NzVbkrYhZ4XWb6fGPjyhgLxapUhXszv7ehQYrQWDgDX4nYWcNcbgWcM2RhYo9s2mbZcfZJ8t5LzYcr24FK79zVybsw5Qj3Rtqug8jpJMy",
        "pubs": [
