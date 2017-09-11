// Copyright (c) 2012-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <cuckoocache.h>
#include <random.h>
#include <script/sigcache.h>
#include <test/util/random.h>
#include <test/util/setup_common.h>

#include <boost/test/unit_test.hpp>

#include <deque>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>

/** Test Suite for CuckooCache
 *
 *  1. All tests should have a deterministic result (using insecure rand
 *  with deterministic seeds)
 *  2. Some test methods are templated to allow for easier testing
 *  against new versions / comparing
 *  3. Results should be treated as a regression test, i.e., did the behavior
 *  change significantly from what was expected. This can be OK, depending on
 *  the nature of the change, but requires updating the tests to reflect the new
 *  expected behavior. For example improving the hit rate may cause some tests
 *  using BOOST_CHECK_CLOSE to fail.
 *
 */
BOOST_AUTO_TEST_SUITE(cuckoocache_tests);

/* Test that no values not inserted into the cache are read out of it.
 *
 * There are no repeats in the first 200000 InsecureRand256() calls
 */
