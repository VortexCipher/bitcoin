#!/usr/bin/env python3
# Copyright (c) 2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

""" Tests the utxocache:* tracepoint API interface.
    See https://github.com/bitcoin/bitcoin/blob/master/doc/tracing.md#context-utxocache
"""

import ctypes
# Test will be skipped if we don't have bcc installed
try:
    from bcc import BPF, USDT # type: ignore[import]
except ImportError:
    pass
from test_framework.messages import COIN
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import assert_equal
from test_framework.wallet import MiniWallet

utxocache_changes_program = """
#include <uapi/linux/ptrace.h>

typedef signed long long i64;

struct utxocache_change
{
    char        txid[32];
    u32         index;
    u32         height;
    i64         value;
    bool        is_coinbase;
};

BPF_PERF_OUTPUT(utxocache_add);
int trace_utxocache_add(struct pt_regs *ctx) {
    struct utxocache_change add = {};
    bpf_usdt_readarg_p(1, ctx, &add.txid, 32);
    bpf_usdt_readarg(2, ctx, &add.index);
    bpf_usdt_readarg(3, ctx, &add.height);
    bpf_usdt_readarg(4, ctx, &add.value);
