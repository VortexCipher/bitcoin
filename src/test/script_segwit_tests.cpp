// Copyright (c) 2012-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <script/script.h>
#include <test/util/setup_common.h>

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(script_segwit_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(IsPayToWitnessScriptHash_Valid)
{
    uint256 dummy;
    CScript p2wsh;
    p2wsh << OP_0 << ToByteVector(dummy);
