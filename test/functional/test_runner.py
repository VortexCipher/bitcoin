#!/usr/bin/env python3
# Copyright (c) 2014-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Run regression test suite.

This module calls down into individual test cases via subprocess. It will
forward all unrecognized arguments onto the individual test scripts.

For a description of arguments recognized by test scripts, see
`test/functional/test_framework/test_framework.py:BitcoinTestFramework.main`.

"""

import argparse
from collections import deque
import configparser
import csv
import datetime
import os
import pathlib
import platform
import time
import shutil
import signal
import subprocess
import sys
import tempfile
import re
import logging

os.environ["REQUIRE_WALLET_TYPE_SET"] = "1"

# Minimum amount of space to run the tests.
MIN_FREE_SPACE = 1.1 * 1024 * 1024 * 1024
# Additional space to run an extra job.
ADDITIONAL_SPACE_PER_JOB = 100 * 1024 * 1024
# Minimum amount of space required for --nocleanup
MIN_NO_CLEANUP_SPACE = 12 * 1024 * 1024 * 1024

# Formatting. Default colors to empty strings.
DEFAULT, BOLD, GREEN, RED = ("", ""), ("", ""), ("", ""), ("", "")
try:
    # Make sure python thinks it can write unicode to its stdout
    "\u2713".encode("utf_8").decode(sys.stdout.encoding)
    TICK = "✓ "
    CROSS = "✖ "
    CIRCLE = "○ "
except UnicodeDecodeError:
    TICK = "P "
    CROSS = "x "
    CIRCLE = "o "

if platform.system() != 'Windows' or sys.getwindowsversion() >= (10, 0, 14393): #type:ignore
    if platform.system() == 'Windows':
        import ctypes
        kernel32 = ctypes.windll.kernel32  # type: ignore
        ENABLE_VIRTUAL_TERMINAL_PROCESSING = 4
        STD_OUTPUT_HANDLE = -11
        STD_ERROR_HANDLE = -12
        # Enable ascii color control to stdout
        stdout = kernel32.GetStdHandle(STD_OUTPUT_HANDLE)
        stdout_mode = ctypes.c_int32()
        kernel32.GetConsoleMode(stdout, ctypes.byref(stdout_mode))
        kernel32.SetConsoleMode(stdout, stdout_mode.value | ENABLE_VIRTUAL_TERMINAL_PROCESSING)
        # Enable ascii color control to stderr
        stderr = kernel32.GetStdHandle(STD_ERROR_HANDLE)
        stderr_mode = ctypes.c_int32()
        kernel32.GetConsoleMode(stderr, ctypes.byref(stderr_mode))
        kernel32.SetConsoleMode(stderr, stderr_mode.value | ENABLE_VIRTUAL_TERMINAL_PROCESSING)
    # primitive formatting on supported
    # terminal via ANSI escape sequences:
    DEFAULT = ('\033[0m', '\033[0m')
    BOLD = ('\033[0m', '\033[1m')
    GREEN = ('\033[0m', '\033[0;32m')
    RED = ('\033[0m', '\033[0;31m')

TEST_EXIT_PASSED = 0
TEST_EXIT_SKIPPED = 77

TEST_FRAMEWORK_UNIT_TESTS = 'feature_framework_unit_tests.py'

EXTENDED_SCRIPTS = [
    # These tests are not run by default.
    # Longest test should go first, to favor running tests in parallel
    'feature_pruning.py',
    'feature_dbcrash.py',
    'feature_index_prune.py',
    'wallet_pruning.py --legacy-wallet',
]

BASE_SCRIPTS = [
    # Scripts that are run by default.
    # Longest test should go first, to favor running tests in parallel
