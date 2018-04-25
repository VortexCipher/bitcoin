// Copyright (c) 2015-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <scheduler.h>

#include <sync.h>
#include <util/time.h>

#include <cassert>
#include <functional>
#include <utility>

CScheduler::CScheduler() = default;

CScheduler::~CScheduler()
{
    assert(nThreadsServicingQueue == 0);
    if (stopWhenEmpty) assert(taskQueue.empty());
}
