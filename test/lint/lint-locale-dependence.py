#!/usr/bin/env python3
# Copyright (c) 2018-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
#
# Be aware that bitcoind and bitcoin-qt differ in terms of localization: Qt
# opts in to POSIX localization by running setlocale(LC_ALL, "") on startup,
# whereas no such call is made in bitcoind.
#
# Qt runs setlocale(LC_ALL, "") on initialization. This installs the locale
# specified by the user's LC_ALL (or LC_*) environment variable as the new
# C locale.
#
# In contrast, bitcoind does not opt in to localization -- no call to
# setlocale(LC_ALL, "") is made and the environment variables LC_* are
# thus ignored.
#
# This results in situations where bitcoind is guaranteed to be running
# with the classic locale ("C") whereas the locale of bitcoin-qt will vary
# depending on the user's environment variables.
#
# An example: Assuming the environment variable LC_ALL=de_DE then the
# call std::to_string(1.23) will return "1.230000" in bitcoind but
# "1,230000" in bitcoin-qt.
#
