// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_NETBASE_H
#define BITCOIN_NETBASE_H

#include <compat/compat.h>
#include <netaddress.h>
#include <serialize.h>
#include <util/sock.h>
#include <util/threadinterrupt.h>

#include <functional>
#include <memory>
#include <stdint.h>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <vector>

extern int nConnectTimeout;
extern bool fNameLookup;

//! -timeout default
static const int DEFAULT_CONNECT_TIMEOUT = 5000;
//! -dns default
static const int DEFAULT_NAME_LOOKUP = true;

/** Prefix for unix domain socket addresses (which are local filesystem paths) */
const std::string ADDR_PREFIX_UNIX = "unix:";

enum class ConnectionDirection {
    None = 0,
