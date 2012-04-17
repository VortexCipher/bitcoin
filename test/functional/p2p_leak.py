#!/usr/bin/env python3
# Copyright (c) 2017-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test message sending before handshake completion.

Before receiving a VERACK, a node should not send anything but VERSION/VERACK
and feature negotiation messages (WTXIDRELAY, SENDADDRV2).

This test connects to a node and sends it a few messages, trying to entice it
into sending us something it shouldn't."""

import time

from test_framework.messages import (
    msg_getaddr,
    msg_ping,
    msg_version,
