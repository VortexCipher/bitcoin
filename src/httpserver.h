// Copyright (c) 2015-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_HTTPSERVER_H
#define BITCOIN_HTTPSERVER_H

#include <functional>
#include <optional>
#include <span>
#include <string>

namespace util {
class SignalInterrupt;
} // namespace util

static const int DEFAULT_HTTP_THREADS=4;
static const int DEFAULT_HTTP_WORKQUEUE=16;
