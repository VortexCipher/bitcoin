#!/usr/bin/env python3
# Copyright (c) 2019-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

"""Test tx status in case of reorgs while wallet being shutdown.

Wallet txn status rely on block connection/disconnection for its
accuracy. In case of reorgs happening while wallet being shutdown
block updates are not going to be received. At wallet loading, we
