#!/usr/bin/env python3
# Copyright (c) 2020-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Parse message capture binary files.  To be used in conjunction with -capturemessages."""

import argparse
import os
import shutil
import sys
from io import BytesIO
import json
from pathlib import Path
from typing import Any, Optional

sys.path.append(os.path.join(os.path.dirname(__file__), '../../test/functional'))

from test_framework.messages import ser_uint256     # noqa: E402
from test_framework.p2p import MESSAGEMAP           # noqa: E402

TIME_SIZE = 8
