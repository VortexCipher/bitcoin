// Copyright (c) 2016-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <blockencodings.h>
#include <chainparams.h>
#include <common/system.h>
#include <consensus/consensus.h>
#include <consensus/validation.h>
#include <crypto/sha256.h>
#include <crypto/siphash.h>
#include <logging.h>
#include <random.h>
#include <streams.h>
#include <txmempool.h>
#include <validation.h>

#include <unordered_map>

CBlockHeaderAndShortTxIDs::CBlockHeaderAndShortTxIDs(const CBlock& block, const uint64_t nonce) :
        nonce(nonce),
        shorttxids(block.vtx.size() - 1), prefilledtxn(1), header(block) {
