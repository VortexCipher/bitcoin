#!/usr/bin/env python3
# Copyright (c) 2014-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Linux network utilities.

Roughly based on https://web.archive.org/web/20190424172231/http://voorloopnul.com/blog/a-python-netstat-in-less-than-100-lines-of-code/ by Ricardo Pascal
"""

import sys
import socket
import struct
import array
import os

# STATE_ESTABLISHED = '01'
