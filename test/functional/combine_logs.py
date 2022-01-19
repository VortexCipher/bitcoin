#!/usr/bin/env python3
# Copyright (c) 2017-2021 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Combine logs from multiple bitcoin nodes as well as the test_framework log.

This streams the combined log output to stdout. Use combine_logs.py > outputfile
to write to an outputfile.

If no argument is provided, the most recent test directory will be used."""

import argparse
from collections import defaultdict, namedtuple
import heapq
import itertools
import os
import pathlib
import re
import sys
import tempfile
