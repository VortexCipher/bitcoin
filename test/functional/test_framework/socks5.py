#!/usr/bin/env python3
# Copyright (c) 2015-2019 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Dummy Socks5 server for testing."""

import socket
import threading
import queue
import logging

logger = logging.getLogger("TestFramework.socks5")

# Protocol constants
class Command:
    CONNECT = 0x01
