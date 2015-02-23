// Copyright (c) 2012-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <checkqueue.h>
#include <common/args.h>
#include <sync.h>
#include <test/util/random.h>
#include <test/util/setup_common.h>
#include <util/chaintype.h>
#include <util/time.h>

#include <boost/test/unit_test.hpp>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <unordered_set>
#include <utility>
#include <vector>

/**
 * Identical to TestingSetup but excludes lock contention logging if
 * `DEBUG_LOCKCONTENTION` is defined, as some of these tests are designed to be
 * heavily contested to trigger race conditions or other issues.
 */
struct NoLockLoggingTestingSetup : public TestingSetup {
    NoLockLoggingTestingSetup()
#ifdef DEBUG_LOCKCONTENTION
        : TestingSetup{ChainType::MAIN, {.extra_args = { "-debugexclude=lock" } }} {}
#else
        : TestingSetup{ChainType::MAIN} {}
#endif
};

BOOST_FIXTURE_TEST_SUITE(checkqueue_tests, NoLockLoggingTestingSetup)

static const unsigned int QUEUE_BATCH_SIZE = 128;
static const int SCRIPT_CHECK_THREADS = 3;

