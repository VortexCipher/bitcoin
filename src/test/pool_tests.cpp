// Copyright (c) 2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <memusage.h>
#include <support/allocators/pool.h>
#include <test/util/poolresourcetester.h>
#include <test/util/random.h>
#include <test/util/setup_common.h>

#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

BOOST_FIXTURE_TEST_SUITE(pool_tests, BasicTestingSetup)

