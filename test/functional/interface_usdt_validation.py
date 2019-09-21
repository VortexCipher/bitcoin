#!/usr/bin/env python3
# Copyright (c) 2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

""" Tests the validation:* tracepoint API interface.
    See https://github.com/bitcoin/bitcoin/blob/master/doc/tracing.md#context-validation
"""

import ctypes

# Test will be skipped if we don't have bcc installed
try:
