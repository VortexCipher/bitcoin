// Copyright (c) 2015-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_SCHEDULER_H
#define BITCOIN_SCHEDULER_H

#include <attributes.h>
#include <sync.h>
#include <threadsafety.h>
#include <util/task_runner.h>

#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <functional>
