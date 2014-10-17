// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test/fuzz/util/net.h>

#include <compat/compat.h>
#include <netaddress.h>
#include <node/protocol_version.h>
#include <protocol.h>
#include <test/fuzz/FuzzedDataProvider.h>
#include <test/fuzz/util.h>
#include <test/util/net.h>
#include <util/sock.h>
#include <util/time.h>

#include <array>
#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>

class CNode;

CNetAddr ConsumeNetAddr(FuzzedDataProvider& fuzzed_data_provider, FastRandomContext* rand) noexcept
{
    struct NetAux {
        Network net;
        CNetAddr::BIP155Network bip155;
        size_t len;
    };

    static constexpr std::array<NetAux, 6> nets{
        NetAux{.net = Network::NET_IPV4, .bip155 = CNetAddr::BIP155Network::IPV4, .len = ADDR_IPV4_SIZE},
        NetAux{.net = Network::NET_IPV6, .bip155 = CNetAddr::BIP155Network::IPV6, .len = ADDR_IPV6_SIZE},
        NetAux{.net = Network::NET_ONION, .bip155 = CNetAddr::BIP155Network::TORV3, .len = ADDR_TORV3_SIZE},
        NetAux{.net = Network::NET_I2P, .bip155 = CNetAddr::BIP155Network::I2P, .len = ADDR_I2P_SIZE},
        NetAux{.net = Network::NET_CJDNS, .bip155 = CNetAddr::BIP155Network::CJDNS, .len = ADDR_CJDNS_SIZE},
        NetAux{.net = Network::NET_INTERNAL, .bip155 = CNetAddr::BIP155Network{0}, .len = 0},
