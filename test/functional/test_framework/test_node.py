#!/usr/bin/env python3
# Copyright (c) 2017-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Class for bitcoind node under test"""

import contextlib
import decimal
import errno
from enum import Enum
import http.client
import json
import logging
import os
import platform
import re
import subprocess
import tempfile
import time
import urllib.parse
import collections
import shlex
from pathlib import Path

from .authproxy import (
    JSONRPCException,
    serialization_fallback,
)
from .descriptors import descsum_create
from .messages import NODE_P2P_V2
from .p2p import P2P_SERVICES, P2P_SUBVERSION
from .util import (
    MAX_NODES,
    assert_equal,
    append_config,
    delete_cookie_file,
    get_auth_cookie,
    get_rpc_proxy,
    rpc_url,
    wait_until_helper_internal,
    p2p_port,
    tor_port,
)

BITCOIND_PROC_WAIT_TIMEOUT = 60


class FailedToStartError(Exception):
    """Raised when a node fails to start correctly."""


class ErrorMatch(Enum):
    FULL_TEXT = 1
    FULL_REGEX = 2
    PARTIAL_REGEX = 3


class TestNode():
    """A class for representing a bitcoind node under test.

    This class contains:

    - state about the node (whether it's running, etc)
    - a Python subprocess.Popen object representing the running process
    - an RPC connection to the node
    - one or more P2P connections to the node


    To make things easier for the test writer, any unrecognised messages will
    be dispatched to the RPC connection."""

    def __init__(self, i, datadir_path, *, chain, rpchost, timewait, timeout_factor, bitcoind, bitcoin_cli, coverage_dir, cwd, extra_conf=None, extra_args=None, use_cli=False, start_perf=False, use_valgrind=False, version=None, descriptors=False, v2transport=False):
        """
        Kwargs:
            start_perf (bool): If True, begin profiling the node with `perf` as soon as
                the node starts.
        """

        self.index = i
        self.p2p_conn_index = 1
        self.datadir_path = datadir_path
        self.bitcoinconf = self.datadir_path / "bitcoin.conf"
        self.stdout_dir = self.datadir_path / "stdout"
        self.stderr_dir = self.datadir_path / "stderr"
        self.chain = chain
        self.rpchost = rpchost
        self.rpc_timeout = timewait
        self.binary = bitcoind
        self.coverage_dir = coverage_dir
        self.cwd = cwd
        self.descriptors = descriptors
        self.has_explicit_bind = False
        if extra_conf is not None:
            append_config(self.datadir_path, extra_conf)
            # Remember if there is bind=... in the config file.
            self.has_explicit_bind = any(e.startswith("bind=") for e in extra_conf)
        # Most callers will just need to add extra args to the standard list below.
        # For those callers that need more flexibility, they can just set the args property directly.
        # Note that common args are set in the config file (see initialize_datadir)
