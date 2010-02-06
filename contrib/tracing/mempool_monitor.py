#!/usr/bin/env python3
# Copyright (c) 2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

""" Example logging Bitcoin Core mempool events using the mempool:added,
    mempool:removed, mempool:replaced, and mempool:rejected tracepoints. """

import curses
import sys
from datetime import datetime, timezone

from bcc import BPF, USDT

# BCC: The C program to be compiled to an eBPF program (by BCC) and loaded into
# a sandboxed Linux kernel VM.
PROGRAM = """
# include <uapi/linux/ptrace.h>

// The longest rejection reason is 118 chars and is generated in case of SCRIPT_ERR_EVAL_FALSE by
// strprintf("mandatory-script-verify-flag-failed (%s)", ScriptErrorString(check.GetScriptError()))
#define MAX_REJECT_REASON_LENGTH        118
// The longest string returned by RemovalReasonToString() is 'sizelimit'
#define MAX_REMOVAL_REASON_LENGTH       9
#define HASH_LENGTH                     32

struct added_event
{
  u8    hash[HASH_LENGTH];
  s32   vsize;
  s64   fee;
};

struct removed_event
{
  u8    hash[HASH_LENGTH];
  char  reason[MAX_REMOVAL_REASON_LENGTH];
