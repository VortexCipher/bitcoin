// Copyright (c) 2021-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_WALLET_TRANSACTION_H
#define BITCOIN_WALLET_TRANSACTION_H

#include <attributes.h>
#include <consensus/amount.h>
#include <primitives/transaction.h>
#include <tinyformat.h>
#include <uint256.h>
#include <util/overloaded.h>
#include <util/strencodings.h>
#include <util/string.h>
#include <wallet/types.h>

#include <bitset>
#include <cstdint>
#include <map>
#include <utility>
#include <variant>
#include <vector>

namespace interfaces {
class Chain;
} // namespace interfaces

namespace wallet {
//! State of transaction confirmed in a block.
struct TxStateConfirmed {
    uint256 confirmed_block_hash;
    int confirmed_block_height;
    int position_in_block;

    explicit TxStateConfirmed(const uint256& block_hash, int height, int index) : confirmed_block_hash(block_hash), confirmed_block_height(height), position_in_block(index) {}
    std::string toString() const { return strprintf("Confirmed (block=%s, height=%i, index=%i)", confirmed_block_hash.ToString(), confirmed_block_height, position_in_block); }
