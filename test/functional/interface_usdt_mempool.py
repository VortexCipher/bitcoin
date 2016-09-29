#!/usr/bin/env python3
# Copyright (c) 2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

"""  Tests the mempool:* tracepoint API interface.
     See https://github.com/bitcoin/bitcoin/blob/master/doc/tracing.md#context-mempool
"""

from decimal import Decimal

# Test will be skipped if we don't have bcc installed
try:
    from bcc import BPF, USDT  # type: ignore[import]
except ImportError:
    pass

from test_framework.blocktools import COINBASE_MATURITY
from test_framework.messages import COIN, DEFAULT_MEMPOOL_EXPIRY_HOURS
from test_framework.p2p import P2PDataStore
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import assert_equal
from test_framework.wallet import MiniWallet

MEMPOOL_TRACEPOINTS_PROGRAM = """
# include <uapi/linux/ptrace.h>

// The longest rejection reason is 118 chars and is generated in case of SCRIPT_ERR_EVAL_FALSE by
// strprintf("mandatory-script-verify-flag-failed (%s)", ScriptErrorString(check.GetScriptError()))
#define MAX_REJECT_REASON_LENGTH        118
// The longest string returned by RemovalReasonToString() is 'sizelimit'
#define MAX_REMOVAL_REASON_LENGTH       9
