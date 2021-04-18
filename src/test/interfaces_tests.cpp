// Copyright (c) 2020-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparams.h>
#include <consensus/validation.h>
#include <interfaces/chain.h>
#include <test/util/setup_common.h>
#include <script/solver.h>
#include <validation.h>

#include <boost/test/unit_test.hpp>

using interfaces::FoundBlock;

BOOST_FIXTURE_TEST_SUITE(interfaces_tests, TestChain100Setup)
