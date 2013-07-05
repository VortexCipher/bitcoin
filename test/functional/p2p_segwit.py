#!/usr/bin/env python3
# Copyright (c) 2016-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test segwit transactions and blocks on P2P network."""
from decimal import Decimal
import random
import time

from test_framework.blocktools import (
    WITNESS_COMMITMENT_HEADER,
    add_witness_commitment,
    create_block,
    create_coinbase,
)
from test_framework.messages import (
    MAX_BIP125_RBF_SEQUENCE,
    CBlockHeader,
    CInv,
    COutPoint,
    CTransaction,
    CTxIn,
    CTxInWitness,
    CTxOut,
    CTxWitness,
    MAX_BLOCK_WEIGHT,
    MSG_BLOCK,
    MSG_TX,
    MSG_WITNESS_FLAG,
    MSG_WITNESS_TX,
    MSG_WTX,
    NODE_NETWORK,
    NODE_WITNESS,
    msg_no_witness_block,
    msg_getdata,
    msg_headers,
    msg_inv,
    msg_tx,
    msg_block,
    msg_no_witness_tx,
    ser_uint256,
    ser_vector,
    sha256,
)
from test_framework.p2p import (
    P2PInterface,
    p2p_lock,
    P2P_SERVICES,
)
from test_framework.script import (
    CScript,
    CScriptNum,
    CScriptOp,
    MAX_SCRIPT_ELEMENT_SIZE,
    OP_0,
    OP_1,
    OP_2,
    OP_16,
    OP_2DROP,
    OP_CHECKMULTISIG,
    OP_CHECKSIG,
    OP_DROP,
    OP_ELSE,
    OP_ENDIF,
    OP_IF,
    OP_RETURN,
    OP_TRUE,
    SIGHASH_ALL,
    SIGHASH_ANYONECANPAY,
    SIGHASH_NONE,
    SIGHASH_SINGLE,
    hash160,
    sign_input_legacy,
    sign_input_segwitv0,
)
from test_framework.script_util import (
    key_to_p2pk_script,
    key_to_p2wpkh_script,
    keyhash_to_p2pkh_script,
    script_to_p2sh_script,
    script_to_p2wsh_script,
)
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    softfork_active,
    assert_raises_rpc_error,
)
from test_framework.wallet import MiniWallet
from test_framework.wallet_util import generate_keypair


MAX_SIGOP_COST = 80000

SEGWIT_HEIGHT = 120

class UTXO():
    """Used to keep track of anyone-can-spend outputs that we can use in the tests."""
    def __init__(self, sha256, n, value):
        self.sha256 = sha256
        self.n = n
        self.nValue = value


def subtest(func):
    """Wraps the subtests for logging and state assertions."""
    def func_wrapper(self, *args, **kwargs):
        self.log.info("Subtest: {} (Segwit active = {})".format(func.__name__, self.segwit_active))
        # Assert segwit status is as expected
        assert_equal(softfork_active(self.nodes[0], 'segwit'), self.segwit_active)
        func(self, *args, **kwargs)
        # Each subtest should leave some utxos for the next subtest
        assert self.utxo
        self.sync_blocks()
        # Assert segwit status is as expected at end of subtest
        assert_equal(softfork_active(self.nodes[0], 'segwit'), self.segwit_active)

    return func_wrapper


def sign_p2pk_witness_input(script, tx_to, in_idx, hashtype, value, key):
    """Add signature for a P2PK witness script."""
    tx_to.wit.vtxinwit[in_idx].scriptWitness.stack = [script]
    sign_input_segwitv0(tx_to, in_idx, script, value, key, hashtype)

def test_transaction_acceptance(node, p2p, tx, with_witness, accepted, reason=None):
    """Send a transaction to the node and check that it's accepted to the mempool

    - Submit the transaction over the p2p interface
    - use the getrawmempool rpc to check for acceptance."""
    reason = [reason] if reason else []
    with node.assert_debug_log(expected_msgs=reason):
        p2p.send_and_ping(msg_tx(tx) if with_witness else msg_no_witness_tx(tx))
        assert_equal(tx.hash in node.getrawmempool(), accepted)


def test_witness_block(node, p2p, block, accepted, with_witness=True, reason=None):
    """Send a block to the node and check that it's accepted

    - Submit the block over the p2p interface
    - use the getbestblockhash rpc to check for acceptance."""
    reason = [reason] if reason else []
    with node.assert_debug_log(expected_msgs=reason):
        p2p.send_and_ping(msg_block(block) if with_witness else msg_no_witness_block(block))
        assert_equal(node.getbestblockhash() == block.hash, accepted)


class TestP2PConn(P2PInterface):
    def __init__(self, wtxidrelay=False):
        super().__init__(wtxidrelay=wtxidrelay)
        self.getdataset = set()
        self.last_wtxidrelay = []
        self.lastgetdata = []
        self.wtxidrelay = wtxidrelay

    # Don't send getdata message replies to invs automatically.
    # We'll send the getdata messages explicitly in the test logic.
    def on_inv(self, message):
        pass

    def on_getdata(self, message):
        self.lastgetdata = message.inv
        for inv in message.inv:
            self.getdataset.add(inv.hash)

    def on_wtxidrelay(self, message):
        self.last_wtxidrelay.append(message)

    def announce_tx_and_wait_for_getdata(self, tx, success=True, use_wtxid=False):
        if success:
            # sanity check
            assert (self.wtxidrelay and use_wtxid) or (not self.wtxidrelay and not use_wtxid)
        with p2p_lock:
            self.last_message.pop("getdata", None)
        if use_wtxid:
            wtxid = tx.calc_sha256(True)
            self.send_message(msg_inv(inv=[CInv(MSG_WTX, wtxid)]))
        else:
            self.send_message(msg_inv(inv=[CInv(MSG_TX, tx.sha256)]))

        if success:
            if use_wtxid:
                self.wait_for_getdata([wtxid])
            else:
                self.wait_for_getdata([tx.sha256])
        else:
            time.sleep(5)
            assert not self.last_message.get("getdata")

    def announce_block_and_wait_for_getdata(self, block, use_header, timeout=60):
        with p2p_lock:
            self.last_message.pop("getdata", None)
        msg = msg_headers()
        msg.headers = [CBlockHeader(block)]
        if use_header:
            self.send_message(msg)
        else:
            self.send_message(msg_inv(inv=[CInv(MSG_BLOCK, block.sha256)]))
            self.wait_for_getheaders(block_hash=block.hashPrevBlock, timeout=timeout)
            self.send_message(msg)
        self.wait_for_getdata([block.sha256], timeout=timeout)

    def request_block(self, blockhash, inv_type, timeout=60):
        with p2p_lock:
            self.last_message.pop("block", None)
        self.send_message(msg_getdata(inv=[CInv(inv_type, blockhash)]))
        self.wait_for_block(blockhash, timeout=timeout)
