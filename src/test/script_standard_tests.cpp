// Copyright (c) 2017-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test/data/bip341_wallet_vectors.json.h>

#include <key.h>
#include <key_io.h>
#include <script/script.h>
#include <script/signingprovider.h>
#include <script/solver.h>
#include <test/util/setup_common.h>
#include <util/strencodings.h>

#include <boost/test/unit_test.hpp>

#include <univalue.h>


BOOST_FIXTURE_TEST_SUITE(script_standard_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(dest_default_is_no_dest)
{
    CTxDestination dest;
    BOOST_CHECK(!IsValidDestination(dest));
}

BOOST_AUTO_TEST_CASE(script_standard_Solver_success)
{
    CKey keys[3];
    CPubKey pubkeys[3];
    for (int i = 0; i < 3; i++) {
        keys[i].MakeNewKey(true);
        pubkeys[i] = keys[i].GetPubKey();
    }

    CScript s;
    std::vector<std::vector<unsigned char> > solutions;

    // TxoutType::PUBKEY
    s.clear();
    s << ToByteVector(pubkeys[0]) << OP_CHECKSIG;
    BOOST_CHECK_EQUAL(Solver(s, solutions), TxoutType::PUBKEY);
    BOOST_CHECK_EQUAL(solutions.size(), 1U);
    BOOST_CHECK(solutions[0] == ToByteVector(pubkeys[0]));
