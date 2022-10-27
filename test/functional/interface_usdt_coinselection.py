#!/usr/bin/env python3
# Copyright (c) 2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

"""  Tests the coin_selection:* tracepoint API interface.
     See https://github.com/bitcoin/bitcoin/blob/master/doc/tracing.md#context-coin_selection
"""

# Test will be skipped if we don't have bcc installed
try:
    from bcc import BPF, USDT # type: ignore[import]
except ImportError:
    pass
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    assert_greater_than,
    assert_raises_rpc_error,
)

coinselection_tracepoints_program = """
#include <uapi/linux/ptrace.h>
