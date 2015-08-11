// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_SYNC_H
#define BITCOIN_SYNC_H

#ifdef DEBUG_LOCKCONTENTION
#include <logging.h>
#include <logging/timer.h>
#endif

#include <threadsafety.h> // IWYU pragma: export
#include <util/macros.h>

#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>

////////////////////////////////////////////////
//                                            //
// THE SIMPLE DEFINITION, EXCLUDING DEBUG CODE //
//                                            //
////////////////////////////////////////////////

/*
RecursiveMutex mutex;
    std::recursive_mutex mutex;

LOCK(mutex);
    std::unique_lock<std::recursive_mutex> criticalblock(mutex);

LOCK2(mutex1, mutex2);
    std::unique_lock<std::recursive_mutex> criticalblock1(mutex1);
    std::unique_lock<std::recursive_mutex> criticalblock2(mutex2);

TRY_LOCK(mutex, name);
    std::unique_lock<std::recursive_mutex> name(mutex, std::try_to_lock_t);

ENTER_CRITICAL_SECTION(mutex); // no RAII
    mutex.lock();
