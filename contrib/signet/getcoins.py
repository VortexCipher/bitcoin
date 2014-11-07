#!/usr/bin/env python3
# Copyright (c) 2020-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

import argparse
import io
import requests
import subprocess
import sys
import xml.etree.ElementTree

DEFAULT_GLOBAL_FAUCET = 'https://signetfaucet.com/claim'
DEFAULT_GLOBAL_CAPTCHA = 'https://signetfaucet.com/captcha'
GLOBAL_FIRST_BLOCK_HASH = '00000086d6b2636cb2a392d45edc4ec544a10024d30141c9adf4bfd9de533b53'
