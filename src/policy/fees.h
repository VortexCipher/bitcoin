// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef BITCOIN_POLICY_FEES_H
#define BITCOIN_POLICY_FEES_H

#include <consensus/amount.h>
#include <policy/feerate.h>
#include <random.h>
#include <sync.h>
#include <threadsafety.h>
#include <uint256.h>
#include <util/fs.h>
#include <validationinterface.h>

#include <array>
#include <chrono>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>


// How often to flush fee estimates to fee_estimates.dat.
static constexpr std::chrono::hours FEE_FLUSH_INTERVAL{1};

/** fee_estimates.dat that are more than 60 hours (2.5 days) old will not be read,
 * as fee estimates are based on historical data and may be inaccurate if
 * network activity has changed.
 */
static constexpr std::chrono::hours MAX_FILE_AGE{60};

