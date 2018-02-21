// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_TEST_FUZZ_UTIL_NET_H
#define BITCOIN_TEST_FUZZ_UTIL_NET_H

#include <net.h>
#include <net_permissions.h>
#include <netaddress.h>
#include <node/connection_types.h>
#include <node/eviction.h>
#include <protocol.h>
#include <test/fuzz/FuzzedDataProvider.h>
