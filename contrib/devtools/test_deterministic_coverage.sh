#!/usr/bin/env bash
#
# Copyright (c) 2019-2020 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
#
# Test for deterministic coverage across unit test runs.

export LC_ALL=C

# Use GCOV_EXECUTABLE="gcov" if compiling with gcc.
# Use GCOV_EXECUTABLE="llvm-cov gcov" if compiling with clang.
GCOV_EXECUTABLE="gcov"

# Disable tests known to cause non-deterministic behaviour and document the source or point of non-determinism.
