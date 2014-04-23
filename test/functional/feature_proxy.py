#!/usr/bin/env python3
# Copyright (c) 2015-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test bitcoind with different proxy configuration.

Test plan:
- Start bitcoind's with different proxy configurations
- Use addnode to initiate connections
- Verify that proxies are connected to, and the right connection command is given
- Proxy configurations to test on bitcoind side:
    - `-proxy` (proxy everything)
    - `-onion` (proxy just onions)
    - `-proxyrandomize` Circuit randomization
    - `-cjdnsreachable`
- Proxy configurations to test on proxy side,
    - support no authentication (other proxy)
    - support no authentication + user/pass authentication (Tor)
    - proxy on IPv6
    - proxy over unix domain sockets

- Create various proxies (as threads)
- Create nodes that connect to them
- Manipulate the peer connections using addnode (onetry) and observe effects
- Test the getpeerinfo `network` field for the peer

addnode connect to IPv4
addnode connect to IPv6
addnode connect to onion
addnode connect to generic DNS name
addnode connect to a CJDNS address

- Test getnetworkinfo for each node

- Test passing invalid -proxy
- Test passing invalid -onion
- Test passing invalid -i2psam
- Test passing -onlynet=onion without -proxy or -onion
- Test passing -onlynet=onion with -onion=0 and with -noonion
- Test passing unknown -onlynet
"""

import os
import socket
import tempfile

