// Copyright (c) 2015-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_TEST_UTIL_SETUP_COMMON_H
#define BITCOIN_TEST_UTIL_SETUP_COMMON_H

#include <common/args.h> // IWYU pragma: export
#include <kernel/context.h>
#include <key.h>
#include <node/caches.h>
#include <node/context.h> // IWYU pragma: export
#include <primitives/transaction.h>
#include <pubkey.h>
#include <stdexcept>
#include <util/chaintype.h> // IWYU pragma: export
#include <util/check.h>
#include <util/fs.h>
#include <util/signalinterrupt.h>
#include <util/string.h>
#include <util/vector.h>

#include <functional>
#include <type_traits>
#include <vector>

