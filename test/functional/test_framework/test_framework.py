#!/usr/bin/env python3
# Copyright (c) 2014-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Base class for RPC testing."""

import configparser
from enum import Enum
import argparse
import logging
import os
import platform
import pdb
import random
import re
import shutil
import subprocess
import sys
import tempfile
import time

from .address import create_deterministic_address_bcrt1_p2tr_op_true
from .authproxy import JSONRPCException
from . import coverage
from .p2p import NetworkThread
from .test_node import TestNode
from .util import (
    MAX_NODES,
    PortSeed,
    assert_equal,
    check_json_precision,
    find_vout_for_address,
    get_datadir_path,
    initialize_datadir,
    p2p_port,
    wait_until_helper_internal,
)


class TestStatus(Enum):
    PASSED = 1
    FAILED = 2
    SKIPPED = 3

TEST_EXIT_PASSED = 0
TEST_EXIT_FAILED = 1
TEST_EXIT_SKIPPED = 77

TMPDIR_PREFIX = "bitcoin_func_test_"


class SkipTest(Exception):
    """This exception is raised to skip a test"""

    def __init__(self, message):
        self.message = message


class BitcoinTestMetaClass(type):
    """Metaclass for BitcoinTestFramework.

    Ensures that any attempt to register a subclass of `BitcoinTestFramework`
    adheres to a standard whereby the subclass overrides `set_test_params` and
    `run_test` but DOES NOT override either `__init__` or `main`. If any of
    those standards are violated, a ``TypeError`` is raised."""

    def __new__(cls, clsname, bases, dct):
        if not clsname == 'BitcoinTestFramework':
            if not ('run_test' in dct and 'set_test_params' in dct):
                raise TypeError("BitcoinTestFramework subclasses must override "
                                "'run_test' and 'set_test_params'")
            if '__init__' in dct or 'main' in dct:
                raise TypeError("BitcoinTestFramework subclasses may not override "
                                "'__init__' or 'main'")

        return super().__new__(cls, clsname, bases, dct)


class BitcoinTestFramework(metaclass=BitcoinTestMetaClass):
    """Base class for a bitcoin test script.

    Individual bitcoin test scripts should subclass this class and override the set_test_params() and run_test() methods.

    Individual tests can also override the following methods to customize the test setup:

    - add_options()
    - setup_chain()
    - setup_network()
    - setup_nodes()

    The __init__() and main() methods should not be overridden.

    This class also contains various public and private helper methods."""

    def __init__(self, test_file) -> None:
        """Sets test framework defaults. Do not override this method. Instead, override the set_test_params() method"""
        self.chain: str = 'regtest'
        self.setup_clean_chain: bool = False
        self.noban_tx_relay: bool = False
        self.nodes: list[TestNode] = []
        self.extra_args = None
        self.network_thread = None
        self.rpc_timeout = 60  # Wait for up to 60 seconds for the RPC server to respond
        self.supports_cli = True
        self.bind_to_localhost_only = True
