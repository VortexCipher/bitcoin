#!/usr/bin/env python3
# Copyright (c) 2013-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
#
# Generate seeds.txt from Pieter's DNS seeder
#

import argparse
import collections
import ipaddress
from pathlib import Path
import re
import sys
from typing import Union

asmap_dir = Path(__file__).parent.parent / "asmap"
sys.path.append(str(asmap_dir))
from asmap import ASMap, net_to_prefix  # noqa: E402

NSEEDS=512

MAX_SEEDS_PER_ASN = {
    'ipv4': 2,
    'ipv6': 10,
