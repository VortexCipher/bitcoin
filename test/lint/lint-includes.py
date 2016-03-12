#!/usr/bin/env python3
#
# Copyright (c) 2018-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
#
# Check for duplicate includes.
# Guard against accidental introduction of new Boost dependencies.
# Check includes: Check for duplicate includes. Enforce bracket syntax includes.

import os
import re
import sys

from subprocess import check_output, CalledProcessError

from lint_ignore_dirs import SHARED_EXCLUDED_SUBTREES

