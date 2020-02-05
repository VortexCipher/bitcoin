#!/usr/bin/env python3
# Copyright (c) 2015-2021 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Utilities for doing coverage analysis on the RPC interface.

Provides a way to track which RPC commands are exercised during
testing.
"""

import os
