#!/usr/bin/env python3
# Copyright (c) 2018-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Verify commits against a trusted keys list."""
import argparse
import hashlib
import logging
import os
import subprocess
import sys
import time

GIT = os.getenv('GIT', 'git')

def tree_sha512sum(commit='HEAD'):
    """Calculate the Tree-sha512 for the commit.

    This is copied from github-merge.py. See https://github.com/bitcoin-core/bitcoin-maintainer-tools."""

