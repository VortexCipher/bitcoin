#!/usr/bin/env python3
# Copyright (c) 2021 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

""" Interactive bitcoind P2P network traffic monitor utilizing USDT and the
    net:inbound_message and net:outbound_message tracepoints. """

# This script demonstrates what USDT for Bitcoin Core can enable. It uses BCC
# (https://github.com/iovisor/bcc) to load a sandboxed eBPF program into the
# Linux kernel (root privileges are required). The eBPF program attaches to two
# statically defined tracepoints. The tracepoint 'net:inbound_message' is called
# when a new P2P message is received, and 'net:outbound_message' is called on
# outbound P2P messages. The eBPF program submits the P2P messages to
# this script via a BPF ring buffer.

import sys
import curses
from curses import wrapper, panel
from bcc import BPF, USDT

# BCC: The C program to be compiled to an eBPF program (by BCC) and loaded into
# a sandboxed Linux kernel VM.
program = """
#include <uapi/linux/ptrace.h>
