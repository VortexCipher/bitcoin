#!/usr/bin/env python3
# Copyright (c) 2022 Pieter Wuille
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

"""Script to find the optimal parameters for the headerssync module through simulation."""

from math import log, exp, sqrt
from datetime import datetime, timedelta
import random

# Parameters:

# Aim for still working fine at some point in the future. [datetime]
TIME = datetime(2026, 10, 5)

# Expected block interval. [timedelta]
BLOCK_INTERVAL = timedelta(seconds=600)

# The number of headers corresponding to the minchainwork parameter. [headers]
MINCHAINWORK_HEADERS = 804000

# Combined processing bandwidth from all attackers to one victim. [bit/s]
# 6 Gbit/s is approximately the speed at which a single thread of a Ryzen 5950X CPU thread can hash
# headers. In practice, the victim's network bandwidth and network processing overheads probably
# impose a far lower number, but it's a useful upper bound.
ATTACK_BANDWIDTH = 6000000000

# How much additional permanent memory usage are attackers (jointly) allowed to cause in the victim,
# expressed as fraction of the normal memory usage due to mainchain growth, for the duration the
# attack is sustained. [unitless]
# 0.2 means that attackers, while they keep up the attack, can cause permanent memory usage due to
# headers storage to grow at 1.2 header per BLOCK_INTERVAL.
ATTACK_FRACTION = 0.2

