#!/usr/bin/env python3
# Copyright (c) 2018-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test the Partially Signed Transaction RPCs.
"""
from decimal import Decimal
from itertools import product
from random import randbytes

from test_framework.blocktools import (
    MAX_STANDARD_TX_WEIGHT,
)
from test_framework.descriptors import descsum_create
from test_framework.key import H_POINT
from test_framework.messages import (
    COutPoint,
    CTransaction,
    CTxIn,
    CTxOut,
    MAX_BIP125_RBF_SEQUENCE,
    WITNESS_SCALE_FACTOR,
)
from test_framework.psbt import (
    PSBT,
    PSBTMap,
    PSBT_GLOBAL_UNSIGNED_TX,
    PSBT_IN_RIPEMD160,
    PSBT_IN_SHA256,
    PSBT_IN_HASH160,
    PSBT_IN_HASH256,
    PSBT_IN_NON_WITNESS_UTXO,
    PSBT_IN_WITNESS_UTXO,
    PSBT_OUT_TAP_TREE,
)
from test_framework.script import CScript, OP_TRUE
from test_framework.script_util import MIN_STANDARD_TX_NONWITNESS_SIZE
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_approx,
    assert_equal,
    assert_greater_than,
    assert_greater_than_or_equal,
    assert_raises_rpc_error,
    find_vout_for_address,
)
from test_framework.wallet_util import (
    calculate_input_weight,
    generate_keypair,
    get_generate_key,
)

import json
import os


class PSBTTest(BitcoinTestFramework):
    def add_options(self, parser):
        self.add_wallet_options(parser)

    def set_test_params(self):
        self.num_nodes = 3
        self.extra_args = [
            ["-walletrbf=1", "-addresstype=bech32", "-changetype=bech32"], #TODO: Remove address type restrictions once taproot has psbt extensions
            ["-walletrbf=0", "-changetype=legacy"],
            []
        ]
        # whitelist peers to speed up tx relay / mempool sync
        for args in self.extra_args:
            args.append("-whitelist=noban@127.0.0.1")
        self.supports_cli = False

    def skip_test_if_missing_module(self):
        self.skip_if_no_wallet()

    def test_psbt_incomplete_after_invalid_modification(self):
        self.log.info("Check that PSBT is correctly marked as incomplete after invalid modification")
        node = self.nodes[2]
        wallet = node.get_wallet_rpc(self.default_wallet_name)
        address = wallet.getnewaddress()
        wallet.sendtoaddress(address=address, amount=1.0)
        self.generate(node, nblocks=1, sync_fun=lambda: self.sync_all(self.nodes[:2]))

        utxos = wallet.listunspent(addresses=[address])
        psbt = wallet.createpsbt([{"txid": utxos[0]["txid"], "vout": utxos[0]["vout"]}], [{wallet.getnewaddress(): 0.9999}])
        signed_psbt = wallet.walletprocesspsbt(psbt)["psbt"]

        # Modify the raw transaction by changing the output address, so the signature is no longer valid
        signed_psbt_obj = PSBT.from_base64(signed_psbt)
        substitute_addr = wallet.getnewaddress()
        raw = wallet.createrawtransaction([{"txid": utxos[0]["txid"], "vout": utxos[0]["vout"]}], [{substitute_addr: 0.9999}])
        signed_psbt_obj.g.map[PSBT_GLOBAL_UNSIGNED_TX] = bytes.fromhex(raw)

        # Check that the walletprocesspsbt call succeeds but also recognizes that the transaction is not complete
        signed_psbt_incomplete = wallet.walletprocesspsbt(signed_psbt_obj.to_base64(), finalize=False)
        assert signed_psbt_incomplete["complete"] is False

    def test_utxo_conversion(self):
        self.log.info("Check that non-witness UTXOs are removed for segwit v1+ inputs")
        mining_node = self.nodes[2]
        offline_node = self.nodes[0]
        online_node = self.nodes[1]

        # Disconnect offline node from others
        # Topology of test network is linear, so this one call is enough
