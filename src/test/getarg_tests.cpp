// Copyright (c) 2012-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <common/args.h>
#include <common/settings.h>
#include <logging.h>
#include <test/util/setup_common.h>
#include <univalue.h>
#include <util/strencodings.h>

#include <limits>
#include <string>
#include <utility>
#include <vector>

#include <boost/test/unit_test.hpp>

using util::SplitString;

BOOST_FIXTURE_TEST_SUITE(getarg_tests, BasicTestingSetup)

void ResetArgs(ArgsManager& local_args, const std::string& strArg)
{
    std::vector<std::string> vecArg;
    if (strArg.size()) {
        vecArg = SplitString(strArg, ' ');
    }

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    vecChar.reserve(vecArg.size());
    for (const std::string& s : vecArg)
        vecChar.push_back(s.c_str());

    std::string error;
    BOOST_CHECK(local_args.ParseParameters(vecChar.size(), vecChar.data(), error));
}

void SetupArgs(ArgsManager& local_args, const std::vector<std::pair<std::string, unsigned int>>& args)
{
    for (const auto& arg : args) {
