#!/usr/bin/env python3
#
# Copyright (c) 2018-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
#
# Lint format strings: This program checks that the number of arguments passed
# to a variadic format string function matches the number of format specifiers
# in the format string.

import argparse
import re
import sys

FALSE_POSITIVES = [
    ("src/dbwrapper.cpp", "vsnprintf(p, limit - p, format, backup_ap)"),
    ("src/index/base.cpp", "FatalErrorf(const char* fmt, const Args&... args)"),
    ("src/index/base.h", "FatalErrorf(const char* fmt, const Args&... args)"),
    ("src/netbase.cpp", "LogConnectFailure(bool manual_connection, const char* fmt, const Args&... args)"),
    ("src/clientversion.cpp", "strprintf(_(COPYRIGHT_HOLDERS).translated, COPYRIGHT_HOLDERS_SUBSTITUTION)"),
    ("src/test/translation_tests.cpp", "strprintf(format, arg)"),
    ("src/validationinterface.cpp", "LogPrint(BCLog::VALIDATION, fmt \"\\n\", __VA_ARGS__)"),
    ("src/wallet/wallet.h", "WalletLogPrintf(const char* fmt, Params... parameters)"),
    ("src/wallet/wallet.h", "LogPrintf((\"%s \" + std::string{fmt}).c_str(), GetDisplayName(), parameters...)"),
    ("src/wallet/scriptpubkeyman.h", "WalletLogPrintf(const char* fmt, Params... parameters)"),
    ("src/wallet/scriptpubkeyman.h", "LogPrintf((\"%s \" + std::string{fmt}).c_str(), m_storage.GetDisplayName(), parameters...)"),
]


def parse_function_calls(function_name, source_code):
