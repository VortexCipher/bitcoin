#!/usr/bin/env python3
# Copyright (c) 2015-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test processing of unrequested blocks.

Setup: two nodes, node0 + node1, not connected to each other. Node1 will have
nMinimumChainWork set to 0x10, so it won't process low-work unrequested blocks.

We have one P2PInterface connection to node0 called test_node, and one to node1
called min_work_node.

The test:
1. Generate one block on each node, to leave IBD.

2. Mine a new block on each tip, and deliver to each node from node's peer.
   The tip should advance for node0, but node1 should skip processing due to
   nMinimumChainWork.

Node1 is unused in tests 3-7:

3. Mine a block that forks from the genesis block, and deliver to test_node.
   Node0 should not process this block (just accept the header), because it
   is unrequested and doesn't have more or equal work to the tip.

4a,b. Send another two blocks that build on the forking block.
   Node0 should process the second block but be stuck on the shorter chain,
   because it's missing an intermediate block.

