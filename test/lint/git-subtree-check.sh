#!/bin/sh
# Copyright (c) 2015-2021 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

export LC_ALL=C

check_remote=0
while getopts "?hr" opt; do
  case $opt in
    '?' | h)
      echo "Usage: $0 [-r] DIR [COMMIT]"
      echo "       $0 -?"
