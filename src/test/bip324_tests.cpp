// Copyright (c) 2023 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bip324.h>
#include <chainparams.h>
#include <key.h>
#include <pubkey.h>
#include <span.h>
#include <test/util/random.h>
#include <test/util/setup_common.h>
#include <util/strencodings.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include <boost/test/unit_test.hpp>

namespace {

void TestBIP324PacketVector(
    uint32_t in_idx,
    const std::string& in_priv_ours_hex,
    const std::string& in_ellswift_ours_hex,
    const std::string& in_ellswift_theirs_hex,
    bool in_initiating,
    const std::string& in_contents_hex,
    uint32_t in_multiply,
