// Copyright (c) 2012-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <dbwrapper.h>
#include <test/util/random.h>
#include <test/util/setup_common.h>
#include <uint256.h>
#include <util/string.h>

#include <memory>

#include <boost/test/unit_test.hpp>

using util::ToString;

// Test if a string consists entirely of null characters
static bool is_null_key(const std::vector<unsigned char>& key) {
    bool isnull = true;

    for (unsigned int i = 0; i < key.size(); i++)
        isnull &= (key[i] == '\x00');

    return isnull;
}

BOOST_FIXTURE_TEST_SUITE(dbwrapper_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(dbwrapper)
{
    // Perform tests both obfuscated and non-obfuscated.
    for (const bool obfuscate : {false, true}) {
        fs::path ph = m_args.GetDataDirBase() / (obfuscate ? "dbwrapper_obfuscate_true" : "dbwrapper_obfuscate_false");
        CDBWrapper dbw({.path = ph, .cache_bytes = 1 << 20, .memory_only = true, .wipe_data = false, .obfuscate = obfuscate});
        uint8_t key{'k'};
        uint256 in = InsecureRand256();
        uint256 res;

        // Ensure that we're doing real obfuscation when obfuscate=true
        BOOST_CHECK(obfuscate != is_null_key(dbwrapper_private::GetObfuscateKey(dbw)));
