// Copyright (c) 2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_HEADERSSYNC_H
#define BITCOIN_HEADERSSYNC_H

#include <arith_uint256.h>
#include <chain.h>
#include <consensus/params.h>
#include <net.h> // For NodeId
#include <primitives/block.h>
#include <uint256.h>
#include <util/bitdeque.h>
#include <util/hasher.h>

#include <deque>
#include <vector>

// A compressed CBlockHeader, which leaves out the prevhash
struct CompressedHeader {
    // header
    int32_t nVersion{0};
    uint256 hashMerkleRoot;
    uint32_t nTime{0};
    uint32_t nBits{0};
    uint32_t nNonce{0};
