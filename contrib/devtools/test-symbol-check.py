#!/usr/bin/env python3
# Copyright (c) 2020-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
'''
Test script for symbol-check.py
'''
import os
import subprocess
import unittest

from utils import determine_wellknown_cmd

def call_symbol_check(cxx: list[str], source, executable, options):
    # This should behave the same as AC_TRY_LINK, so arrange well-known flags
    # in the same order as autoconf would.
    #
