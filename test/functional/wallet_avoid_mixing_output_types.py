#!/usr/bin/env python3
# Copyright (c) 2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or https://www.opensource.org/licenses/mit-license.php.
"""Test output type mixing during coin selection

A wallet may have different types of UTXOs to choose from during coin selection,
where output type is one of the following:
    - BECH32M
    - BECH32
    - P2SH-SEGWIT
    - LEGACY

This test verifies that mixing different output types is avoided unless
absolutely necessary. Both wallets start with zero funds. Alice mines
enough blocks to have spendable coinbase outputs. Alice sends three
random value payments which sum to 10BTC for each output type to Bob,
for a total of 40BTC in Bob's wallet.
