// Copyright (c) 2023 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bench/bench.h>
#include <kernel/disconnected_transactions.h>
#include <primitives/block.h>
#include <test/util/random.h>
#include <test/util/setup_common.h>

constexpr size_t BLOCK_VTX_COUNT{4000};
constexpr size_t BLOCK_VTX_COUNT_10PERCENT{400};

