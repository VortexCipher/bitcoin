#!/usr/bin/env python3
#
# Copyright (c) 2018-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
#
# Download or build previous releases.
# Needs curl and tar to download a release, or the build dependencies when
# building a release.

import argparse
import contextlib
from fnmatch import fnmatch
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import hashlib

SHA256_SUMS = {
    "0e2819135366f150d9906e294b61dff58fd1996ebd26c2f8e979d6c0b7a79580": {"tag": "v0.14.3", "tarball": "bitcoin-0.14.3-aarch64-linux-gnu.tar.gz"},
    "d86fc90824a85c38b25c8488115178d5785dbc975f5ff674f9f5716bc8ad6e65": {"tag": "v0.14.3", "tarball": "bitcoin-0.14.3-arm-linux-gnueabihf.tar.gz"},
    "1b0a7408c050e3d09a8be8e21e183ef7ee570385dc41216698cc3ab392a484e7": {"tag": "v0.14.3", "tarball": "bitcoin-0.14.3-osx64.tar.gz"},
    "706e0472dbc933ed2757650d54cbcd780fd3829ebf8f609b32780c7eedebdbc9": {"tag": "v0.14.3", "tarball": "bitcoin-0.14.3-x86_64-linux-gnu.tar.gz"},
    #
    "d40f18b4e43c6e6370ef7db9131f584fbb137276ec2e3dba67a4b267f81cb644": {"tag": "v0.15.2", "tarball": "bitcoin-0.15.2-aarch64-linux-gnu.tar.gz"},
    "54fb877a148a6ad189a1e1ab1ff8b11181e58ff2aaf430da55b3fd46ae549a6b": {"tag": "v0.15.2", "tarball": "bitcoin-0.15.2-arm-linux-gnueabihf.tar.gz"},
    "87e9340ff3d382d543b2b69112376077f0c8b4f7450d372e83b68f5a1e22b2df": {"tag": "v0.15.2", "tarball": "bitcoin-0.15.2-osx64.tar.gz"},
    "566be44190fd76daa01f13d428939dadfb8e3daacefc8fa17f433cad28f73bd5": {"tag": "v0.15.2", "tarball": "bitcoin-0.15.2-x86_64-linux-gnu.tar.gz"},
