// Copyright (c) 2012-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <addrdb.h>
#include <addrman.h>
#include <addrman_impl.h>
#include <chainparams.h>
#include <clientversion.h>
#include <hash.h>
#include <netbase.h>
#include <random.h>
#include <test/data/asmap.raw.h>
#include <test/util/setup_common.h>
#include <util/asmap.h>
#include <util/string.h>

#include <boost/test/unit_test.hpp>

#include <optional>
#include <string>

using namespace std::literals;
using node::NodeContext;
using util::ToString;

static NetGroupManager EMPTY_NETGROUPMAN{std::vector<bool>()};
static const bool DETERMINISTIC{true};

static int32_t GetCheckRatio(const NodeContext& node_ctx)
{
    return std::clamp<int32_t>(node_ctx.args->GetIntArg("-checkaddrman", 100), 0, 1000000);
}

static CNetAddr ResolveIP(const std::string& ip)
{
    const std::optional<CNetAddr> addr{LookupHost(ip, false)};
    BOOST_CHECK_MESSAGE(addr.has_value(), strprintf("failed to resolve: %s", ip));
    return addr.value_or(CNetAddr{});
}

static CService ResolveService(const std::string& ip, uint16_t port = 0)
{
    const std::optional<CService> serv{Lookup(ip, port, false)};
    BOOST_CHECK_MESSAGE(serv.has_value(), strprintf("failed to resolve: %s:%i", ip, port));
    return serv.value_or(CService{});
}


static std::vector<bool> FromBytes(const unsigned char* source, int vector_size)
{
    std::vector<bool> result(vector_size);
    for (int byte_i = 0; byte_i < vector_size / 8; ++byte_i) {
        unsigned char cur_byte = source[byte_i];
        for (int bit_i = 0; bit_i < 8; ++bit_i) {
            result[byte_i * 8 + bit_i] = (cur_byte >> bit_i) & 1;
        }
    }
    return result;
}

BOOST_FIXTURE_TEST_SUITE(addrman_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(addrman_simple)
{
    auto addrman = std::make_unique<AddrMan>(EMPTY_NETGROUPMAN, DETERMINISTIC, GetCheckRatio(m_node));

    CNetAddr source = ResolveIP("252.2.2.2");

    // Test: Does Addrman respond correctly when empty.
    BOOST_CHECK_EQUAL(addrman->Size(), 0U);
    auto addr_null = addrman->Select().first;
    BOOST_CHECK_EQUAL(addr_null.ToStringAddrPort(), "[::]:0");

    // Test: Does Addrman::Add work as expected.
    CService addr1 = ResolveService("250.1.1.1", 8333);
    BOOST_CHECK(addrman->Add({CAddress(addr1, NODE_NONE)}, source));
    BOOST_CHECK_EQUAL(addrman->Size(), 1U);
    auto addr_ret1 = addrman->Select().first;
    BOOST_CHECK_EQUAL(addr_ret1.ToStringAddrPort(), "250.1.1.1:8333");

    // Test: Does IP address deduplication work correctly.
    //  Expected dup IP should not be added.
    CService addr1_dup = ResolveService("250.1.1.1", 8333);
    BOOST_CHECK(!addrman->Add({CAddress(addr1_dup, NODE_NONE)}, source));
    BOOST_CHECK_EQUAL(addrman->Size(), 1U);


    // Test: New table has one addr and we add a diff addr we should
    //  have at least one addr.
    // Note that addrman's size cannot be tested reliably after insertion, as
    // hash collisions may occur. But we can always be sure of at least one
    // success.

    CService addr2 = ResolveService("250.1.1.2", 8333);
    BOOST_CHECK(addrman->Add({CAddress(addr2, NODE_NONE)}, source));
    BOOST_CHECK(addrman->Size() >= 1);

    // Test: reset addrman and test AddrMan::Add multiple addresses works as expected
    addrman = std::make_unique<AddrMan>(EMPTY_NETGROUPMAN, DETERMINISTIC, GetCheckRatio(m_node));
    std::vector<CAddress> vAddr;
    vAddr.emplace_back(ResolveService("250.1.1.3", 8333), NODE_NONE);
    vAddr.emplace_back(ResolveService("250.1.1.4", 8333), NODE_NONE);
    BOOST_CHECK(addrman->Add(vAddr, source));
    BOOST_CHECK(addrman->Size() >= 1);
}

BOOST_AUTO_TEST_CASE(addrman_ports)
{
    auto addrman = std::make_unique<AddrMan>(EMPTY_NETGROUPMAN, DETERMINISTIC, GetCheckRatio(m_node));

    CNetAddr source = ResolveIP("252.2.2.2");

    BOOST_CHECK_EQUAL(addrman->Size(), 0U);

    // Test 7; Addr with same IP but diff port does not replace existing addr.
