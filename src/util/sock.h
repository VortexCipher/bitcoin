// Copyright (c) 2020-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_UTIL_SOCK_H
#define BITCOIN_UTIL_SOCK_H

#include <compat/compat.h>
#include <util/threadinterrupt.h>
#include <util/time.h>

#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>

/**
 * Maximum time to wait for I/O readiness.
 * It will take up until this time to break off in case of an interruption.
 */
static constexpr auto MAX_WAIT_FOR_IO = 1s;

/**
 * RAII helper class that manages a socket and closes it automatically when it goes out of scope.
 */
class Sock
{
public:
