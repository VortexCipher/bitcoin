#!/usr/bin/env python3
# Copyright (c) 2016-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

import re
import fnmatch
import sys
import subprocess
import datetime
import os

################################################################################
# file filtering
################################################################################

EXCLUDE = [
    # auto generated:
    'src/qt/bitcoinstrings.cpp',
    'src/chainparamsseeds.h',
    # other external copyrights:
    'src/test/fuzz/FuzzedDataProvider.h',
    'src/tinyformat.h',
    'src/bench/nanobench.h',
    'test/functional/test_framework/bignum.py',
    # python init:
    '*__init__.py',
]
EXCLUDE_COMPILED = re.compile('|'.join([fnmatch.translate(m) for m in EXCLUDE]))

EXCLUDE_DIRS = [
    # git subtrees
    "src/crypto/ctaes/",
    "src/leveldb/",
    "src/minisketch",
    "src/secp256k1/",
    "src/crc32c/",
]

INCLUDE = ['*.h', '*.cpp', '*.cc', '*.c', '*.mm', '*.py', '*.sh', '*.bash-completion']
INCLUDE_COMPILED = re.compile('|'.join([fnmatch.translate(m) for m in INCLUDE]))

def applies_to_file(filename):
    for excluded_dir in EXCLUDE_DIRS:
        if filename.startswith(excluded_dir):
            return False
    return ((EXCLUDE_COMPILED.match(filename) is None) and
            (INCLUDE_COMPILED.match(filename) is not None))

################################################################################
# obtain list of files in repo according to INCLUDE and EXCLUDE
################################################################################

GIT_LS_CMD = 'git ls-files --full-name'.split(' ')
GIT_TOPLEVEL_CMD = 'git rev-parse --show-toplevel'.split(' ')

def call_git_ls(base_directory):
    out = subprocess.check_output([*GIT_LS_CMD, base_directory])
    return [f for f in out.decode("utf-8").split('\n') if f != '']

