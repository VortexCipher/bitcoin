// Copyright (c) 2011-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <key.h>
#include <policy/policy.h>
#include <script/interpreter.h>
#include <script/script.h>
#include <script/script_error.h>
#include <script/sign.h>
#include <script/signingprovider.h>
#include <test/util/setup_common.h>
#include <tinyformat.h>
#include <uint256.h>


#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(multisig_tests, BasicTestingSetup)

static CScript
sign_multisig(const CScript& scriptPubKey, const std::vector<CKey>& keys, const CTransaction& transaction, int whichIn)
