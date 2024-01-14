#!/usr/bin/env python3
# Copyright 2014 BitPay Inc.
# Copyright 2016-2017 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test framework for bitcoin utils.

Runs automatically during `make check`.

Can also be run manually."""

import argparse
import configparser
import difflib
import json
import logging
import os
import pprint
