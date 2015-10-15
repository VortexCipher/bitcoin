#!/usr/bin/env python3
# Copyright (c) 2020-2021 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test asmap config argument for ASN-based IP bucketing.

Verify node behaviour and debug log when launching bitcoind in these cases:

1. `bitcoind` with no -asmap arg, using /16 prefix for IP bucketing

2. `bitcoind -asmap=<absolute path>`, using the unit test skeleton asmap

3. `bitcoind -asmap=<relative path>`, using the unit test skeleton asmap

4. `bitcoind -asmap/-asmap=` with no file specified, using the default asmap
