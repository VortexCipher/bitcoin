#!/usr/bin/env python3
# Copyright (c) 2014-2021 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
'''
Script to generate list of seed nodes for kernel/chainparams.cpp.

This script expects three text files in the directory that is passed as an
argument:

    nodes_main.txt
    nodes_test.txt
    nodes_testnet4.txt

These files must consist of lines in the format

    <ip>:<port>
    [<ipv6>]:<port>
