#!/usr/bin/env python3
#
# linearize-data.py: Construct a linear, no-fork version of the chain.
#
# Copyright (c) 2013-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
#

import struct
import re
import os
import os.path
import sys
import hashlib
import datetime
import time
import glob
from collections import namedtuple

settings = {}

def calc_hash_str(blk_hdr):
    blk_hdr_hash = hashlib.sha256(hashlib.sha256(blk_hdr).digest()).digest()
    return blk_hdr_hash[::-1].hex()

def get_blk_dt(blk_hdr):
    members = struct.unpack("<I", blk_hdr[68:68+4])
    nTime = members[0]
    dt = datetime.datetime.fromtimestamp(nTime)
    dt_ym = datetime.datetime(dt.year, dt.month, 1)
    return (dt_ym, nTime)
