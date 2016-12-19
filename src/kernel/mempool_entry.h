// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_KERNEL_MEMPOOL_ENTRY_H
#define BITCOIN_KERNEL_MEMPOOL_ENTRY_H

#include <consensus/amount.h>
#include <consensus/validation.h>
#include <core_memusage.h>
#include <policy/policy.h>
#include <policy/settings.h>
#include <primitives/transaction.h>
#include <util/epochguard.h>
#include <util/overflow.h>

#include <chrono>
#include <functional>
#include <memory>
#include <set>
#include <stddef.h>
#include <stdint.h>

class CBlockIndex;

