#!/usr/bin/env python3
# Copyright (c) 2019-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Run fuzz test targets.
"""

from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path
import argparse
import configparser
import logging
import os
import platform
import random
import subprocess
import sys


def get_fuzz_env(*, target, source_dir):
    symbolizer = os.environ.get('LLVM_SYMBOLIZER_PATH', "/usr/bin/llvm-symbolizer")
    fuzz_env = {
        'FUZZ': target,
        'UBSAN_OPTIONS':
        f'suppressions={source_dir}/test/sanitizer_suppressions/ubsan:print_stacktrace=1:halt_on_error=1:report_error_type=1',
        'UBSAN_SYMBOLIZER_PATH':symbolizer,
        "ASAN_OPTIONS": "detect_stack_use_after_return=1:check_initialization_order=1:strict_init_order=1",
        'ASAN_SYMBOLIZER_PATH':symbolizer,
        'MSAN_SYMBOLIZER_PATH':symbolizer,
    }
    if platform.system() == "Windows":
        # On Windows, `env` option must include valid `SystemRoot`.
        fuzz_env = {**fuzz_env, 'SystemRoot': os.environ.get('SystemRoot')}
    return fuzz_env


def main():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        description='''Run the fuzz targets with all inputs from the corpus_dir once.''',
    )
