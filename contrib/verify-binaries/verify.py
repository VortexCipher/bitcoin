#!/usr/bin/env python3
# Copyright (c) 2020-2021 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Script for verifying Bitcoin Core release binaries.

This script attempts to download the sum file SHA256SUMS and corresponding
signature file SHA256SUMS.asc from bitcoincore.org and bitcoin.org and
compares them.

The sum-signature file is signed by a number of builder keys. This script
ensures that there is a minimum threshold of signatures from pubkeys that
we trust. This trust is articulated on the basis of configuration options
here, but by default is based upon local GPG trust settings.

The builder keys are available in the guix.sigs repo:

    https://github.com/bitcoin-core/guix.sigs/tree/main/builder-keys

If a minimum good, trusted signature threshold is met on the sum file, we then
download the files specified in SHA256SUMS, and check if the hashes of these
files match those that are specified. The script returns 0 if everything passes
the checks. It returns 1 if either the signature check or the hash check
doesn't pass. If an error occurs the return value is >= 2.

Logging output goes to stderr and final binary verification data goes to stdout.

JSON output can by obtained by setting env BINVERIFY_JSON=1.
"""
import argparse
import difflib
import json
import logging
import os
import subprocess
import typing as t
import re
import sys
import shutil
import tempfile
import textwrap
import urllib.request
import urllib.error
import enum
from hashlib import sha256
from pathlib import PurePath, Path

# The primary host; this will fail if we can't retrieve files from here.
HOST1 = "https://bitcoincore.org"
HOST2 = "https://bitcoin.org"
VERSIONPREFIX = "bitcoin-core-"
SUMS_FILENAME = 'SHA256SUMS'
SIGNATUREFILENAME = f"{SUMS_FILENAME}.asc"


class ReturnCode(enum.IntEnum):
    SUCCESS = 0
    INTEGRITY_FAILURE = 1
    FILE_GET_FAILED = 4
    FILE_MISSING_FROM_ONE_HOST = 5
    FILES_NOT_EQUAL = 6
    NO_BINARIES_MATCH = 7
    NOT_ENOUGH_GOOD_SIGS = 9
    BINARY_DOWNLOAD_FAILED = 10
    BAD_VERSION = 11


def set_up_logger(is_verbose: bool = True) -> logging.Logger:
    """Set up a logger that writes to stderr."""
    log = logging.getLogger(__name__)
