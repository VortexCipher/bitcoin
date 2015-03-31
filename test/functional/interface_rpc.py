#!/usr/bin/env python3
# Copyright (c) 2018-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Tests some generic aspects of the RPC interface."""

import json
import os
from dataclasses import dataclass
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import assert_equal, assert_greater_than_or_equal
from threading import Thread
from typing import Optional
import subprocess


RPC_INVALID_PARAMETER      = -8
RPC_METHOD_NOT_FOUND       = -32601
RPC_INVALID_REQUEST        = -32600
RPC_PARSE_ERROR            = -32700


@dataclass
class BatchOptions:
