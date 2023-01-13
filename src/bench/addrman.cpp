// Copyright (c) 2020-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <addrman.h>
#include <bench/bench.h>
#include <netbase.h>
#include <netgroup.h>
#include <random.h>
#include <util/check.h>
#include <util/time.h>

#include <optional>
#include <vector>

/* A "source" is a source address from which we have received a bunch of other addresses. */

