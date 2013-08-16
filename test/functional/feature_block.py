#!/usr/bin/env python3
# Copyright (c) 2015-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test block processing."""
import copy
import time

from test_framework.blocktools import (
    create_block,
    create_coinbase,
    create_tx_with_script,
    get_legacy_sigopcount_block,
    MAX_BLOCK_SIGOPS,
)
from test_framework.messages import (
    CBlock,
    COIN,
    COutPoint,
    CTransaction,
    CTxIn,
    CTxOut,
    MAX_BLOCK_WEIGHT,
    SEQUENCE_FINAL,
    uint256_from_compact,
    uint256_from_str,
)
from test_framework.p2p import P2PDataStore
from test_framework.script import (
    CScript,
    MAX_SCRIPT_ELEMENT_SIZE,
    OP_2DUP,
    OP_CHECKMULTISIG,
    OP_CHECKMULTISIGVERIFY,
    OP_CHECKSIG,
    OP_CHECKSIGVERIFY,
    OP_ELSE,
    OP_ENDIF,
    OP_DROP,
    OP_FALSE,
    OP_IF,
    OP_INVALIDOPCODE,
    OP_RETURN,
    OP_TRUE,
    sign_input_legacy,
)
from test_framework.script_util import (
    script_to_p2sh_script,
)
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    assert_greater_than,
)
from test_framework.wallet_util import generate_keypair
from data import invalid_txs


#  Use this class for tests that require behavior other than normal p2p behavior.
#  For now, it is used to serialize a bloated varint (b64).
class CBrokenBlock(CBlock):
    def initialize(self, base_block):
        self.vtx = copy.deepcopy(base_block.vtx)
        self.hashMerkleRoot = self.calc_merkle_root()

    def serialize(self, with_witness=False):
        r = b""
        r += super(CBlock, self).serialize()
        r += (255).to_bytes(1, "little") + len(self.vtx).to_bytes(8, "little")
        for tx in self.vtx:
            if with_witness:
                r += tx.serialize_with_witness()
            else:
                r += tx.serialize_without_witness()
        return r

    def normal_serialize(self):
        return super().serialize()


DUPLICATE_COINBASE_SCRIPT_SIG = b'\x01\x78'  # Valid for block at height 120


class FullBlockTest(BitcoinTestFramework):
    def set_test_params(self):
        self.num_nodes = 1
        self.setup_clean_chain = True
        self.extra_args = [[
            '-acceptnonstdtxn=1',  # This is a consensus block test, we don't care about tx policy
            '-testactivationheight=bip34@2',
        ]]

    def run_test(self):
        node = self.nodes[0]  # convenience reference to the node

        self.bootstrap_p2p()  # Add one p2p connection to the node

        self.block_heights = {}
        self.coinbase_key, self.coinbase_pubkey = generate_keypair()
        self.tip = None
        self.blocks = {}
        self.genesis_hash = int(self.nodes[0].getbestblockhash(), 16)
        self.block_heights[self.genesis_hash] = 0
        self.spendable_outputs = []

        # Create a new block
        b_dup_cb = self.next_block('dup_cb')
        b_dup_cb.vtx[0].vin[0].scriptSig = DUPLICATE_COINBASE_SCRIPT_SIG
        b_dup_cb.vtx[0].rehash()
        duplicate_tx = b_dup_cb.vtx[0]
        b_dup_cb = self.update_block('dup_cb', [])
        self.send_blocks([b_dup_cb])

        b0 = self.next_block(0)
        self.save_spendable_output()
        self.send_blocks([b0])

        # These constants chosen specifically to trigger an immature coinbase spend
        # at a certain time below.
        NUM_BUFFER_BLOCKS_TO_GENERATE = 99
        NUM_OUTPUTS_TO_COLLECT = 33

        # Allow the block to mature
        blocks = []
        for i in range(NUM_BUFFER_BLOCKS_TO_GENERATE):
            blocks.append(self.next_block(f"maturitybuffer.{i}"))
            self.save_spendable_output()
        self.send_blocks(blocks)

        # collect spendable outputs now to avoid cluttering the code later on
        out = []
        for _ in range(NUM_OUTPUTS_TO_COLLECT):
            out.append(self.get_spendable_output())

        # Start by building a couple of blocks on top (which output is spent is
        # in parentheses):
        #     genesis -> b1 (0) -> b2 (1)
        b1 = self.next_block(1, spend=out[0])
        self.save_spendable_output()

        b2 = self.next_block(2, spend=out[1])
        self.save_spendable_output()

        self.send_blocks([b1, b2], timeout=4)
