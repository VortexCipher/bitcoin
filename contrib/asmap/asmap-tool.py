#!/usr/bin/env python3
# Copyright (c) 2022 Pieter Wuille
# Distributed under the MIT software license, see the accompanying
# file LICENSE or http://www.opensource.org/licenses/mit-license.php.

import argparse
import sys
import ipaddress
import json
import math
from collections import defaultdict

import asmap

def load_file(input_file):
    try:
        contents = input_file.read()
    except OSError as err:
        sys.exit(f"Input file '{input_file.name}' cannot be read: {err.strerror}.")
