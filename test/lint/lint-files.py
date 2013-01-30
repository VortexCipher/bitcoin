#!/usr/bin/env python3
# Copyright (c) 2021-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

"""
This checks that all files in the repository have correct filenames and permissions
"""

import os
import re
import sys
from subprocess import check_output
from typing import Optional, NoReturn

CMD_TOP_LEVEL = ["git", "rev-parse", "--show-toplevel"]
CMD_ALL_FILES = ["git", "ls-files", "-z", "--full-name", "--stage"]
CMD_SHEBANG_FILES = ["git", "grep", "--full-name", "--line-number", "-I", "^#!"]

ALL_SOURCE_FILENAMES_REGEXP = r"^.*\.(cpp|h|py|sh)$"
ALLOWED_FILENAME_REGEXP = "^[a-zA-Z0-9/_.@][a-zA-Z0-9/_.@-]*$"
