// Copyright (c) 2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_TXREQUEST_H
#define BITCOIN_TXREQUEST_H

#include <primitives/transaction.h>
#include <net.h> // For NodeId
#include <uint256.h>

#include <chrono>
#include <vector>

#include <stdint.h>

/** Data structure to keep track of, and schedule, transaction downloads from peers.
 *
 * === Specification ===
 *
 * We keep track of which peers have announced which transactions, and use that to determine which requests
