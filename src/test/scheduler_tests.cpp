// Copyright (c) 2012-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <random.h>
#include <scheduler.h>
#include <util/time.h>

#include <boost/test/unit_test.hpp>

#include <functional>
#include <mutex>
#include <thread>
#include <vector>

BOOST_AUTO_TEST_SUITE(scheduler_tests)

static void microTask(CScheduler& s, std::mutex& mutex, int& counter, int delta, std::chrono::steady_clock::time_point rescheduleTime)
{
    {
        std::lock_guard<std::mutex> lock(mutex);
