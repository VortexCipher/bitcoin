#!/usr/bin/env python3
# Copyright (c) 2014-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test the importmulti RPC.

Test importmulti by generating keys on node0, importing the scriptPubKeys and
addresses on node1 and then testing the address info for the different address
variants.

- `get_key()` and `get_multisig()` are called to generate keys on node0 and
  return the privkeys, pubkeys and all variants of scriptPubKey and address.
- `test_importmulti()` is called to send an importmulti call to node1, test
  success, and (if unsuccessful) test the error code and error message returned.
- `test_address()` is called to call getaddressinfo for an address on node1
  and test the values returned."""

from test_framework.blocktools import COINBASE_MATURITY
from test_framework.script import (
    CScript,
    OP_NOP,
)
from test_framework.test_framework import BitcoinTestFramework
from test_framework.descriptors import descsum_create
from test_framework.util import (
    assert_equal,
    assert_greater_than,
    assert_raises_rpc_error,
)
from test_framework.wallet_util import (
    get_key,
    get_multisig,
    test_address,
)


class ImportMultiTest(BitcoinTestFramework):
    def add_options(self, parser):
        self.add_wallet_options(parser, descriptors=False)

    def set_test_params(self):
        self.num_nodes = 2
        self.extra_args = [["-addresstype=legacy"], ["-addresstype=legacy"]]
        self.setup_clean_chain = True

    def skip_test_if_missing_module(self):
        self.skip_if_no_wallet()

    def setup_network(self):
        self.setup_nodes()

    def test_importmulti(self, req, success, error_code=None, error_message=None, warnings=None):
        """Run importmulti and assert success"""
        if warnings is None:
            warnings = []
        result = self.nodes[1].importmulti([req])
        observed_warnings = []
        if 'warnings' in result[0]:
            observed_warnings = result[0]['warnings']
        assert_equal("\n".join(sorted(warnings)), "\n".join(sorted(observed_warnings)))
        assert_equal(result[0]['success'], success)
        if error_code is not None:
            assert_equal(result[0]['error']['code'], error_code)
            assert_equal(result[0]['error']['message'], error_message)

    def run_test(self):
        self.log.info("Mining blocks...")
        self.generate(self.nodes[0], 1, sync_fun=self.no_op)
        self.generate(self.nodes[1], 1, sync_fun=self.no_op)
        timestamp = self.nodes[1].getblock(self.nodes[1].getbestblockhash())['mediantime']

        node0_address1 = self.nodes[0].getaddressinfo(self.nodes[0].getnewaddress())

        # Check only one address
        assert_equal(node0_address1['ismine'], True)

        # Node 1 sync test
        assert_equal(self.nodes[1].getblockcount(), 1)

        # Address Test - before import
        address_info = self.nodes[1].getaddressinfo(node0_address1['address'])
        assert_equal(address_info['iswatchonly'], False)
        assert_equal(address_info['ismine'], False)

        # RPC importmulti -----------------------------------------------

        # Bitcoin Address (implicit non-internal)
        self.log.info("Should import an address")
        key = get_key(self.nodes[0])
        self.test_importmulti({"scriptPubKey": {"address": key.p2pkh_addr},
