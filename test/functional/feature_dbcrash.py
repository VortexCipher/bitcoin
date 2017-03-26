#!/usr/bin/env python3
# Copyright (c) 2017-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test recovery from a crash during chainstate writing.

- 4 nodes
  * node0, node1, and node2 will have different dbcrash ratios, and different
    dbcache sizes
  * node3 will be a regular node, with no crashing.
  * The nodes will not connect to each other.

- use default test framework starting chain. initialize starting_tip_height to
  tip height.

- Main loop:
  * generate lots of transactions on node3, enough to fill up a block.
  * uniformly randomly pick a tip height from starting_tip_height to
    tip_height; with probability 1/(height_difference+4), invalidate this block.
  * mine enough blocks to overtake tip_height at start of loop.
  * for each node in [node0,node1,node2]:
     - for each mined block:
       * submit block to node
       * if node crashed on/after submitting:
         - restart until recovery succeeds
         - check that utxo matches node3 using gettxoutsetinfo"""

import errno
