// Copyright (c) 2019-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparams.h>
#include <hash.h>
#include <net.h>
#include <netmessagemaker.h>
#include <protocol.h>
#include <test/fuzz/FuzzedDataProvider.h>
#include <test/fuzz/fuzz.h>
#include <test/fuzz/util.h>
#include <util/chaintype.h>

#include <cassert>
#include <cstdint>
#include <limits>
#include <optional>
#include <vector>

namespace {

auto g_all_messages = ALL_NET_MESSAGE_TYPES;

void initialize_p2p_transport_serialization()
{
    static ECC_Context ecc_context{};
    SelectParams(ChainType::REGTEST);
    std::sort(g_all_messages.begin(), g_all_messages.end());
}

} // namespace

FUZZ_TARGET(p2p_transport_serialization, .init = initialize_p2p_transport_serialization)
{
    // Construct transports for both sides, with dummy NodeIds.
    V1Transport recv_transport{NodeId{0}};
    V1Transport send_transport{NodeId{1}};

    FuzzedDataProvider fuzzed_data_provider{buffer.data(), buffer.size()};
