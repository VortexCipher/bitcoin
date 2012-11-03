// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <config/bitcoin-config.h> // IWYU pragma: keep

#include <netbase.h>

#include <compat/compat.h>
#include <logging.h>
#include <sync.h>
#include <tinyformat.h>
#include <util/sock.h>
#include <util/strencodings.h>
#include <util/string.h>
#include <util/time.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>

#ifdef HAVE_SOCKADDR_UN
#include <sys/un.h>
#endif

using util::ContainsNoNUL;

// Settings
static GlobalMutex g_proxyinfo_mutex;
static Proxy proxyInfo[NET_MAX] GUARDED_BY(g_proxyinfo_mutex);
static Proxy nameProxy GUARDED_BY(g_proxyinfo_mutex);
int nConnectTimeout = DEFAULT_CONNECT_TIMEOUT;
bool fNameLookup = DEFAULT_NAME_LOOKUP;

// Need ample time for negotiation for very slow proxies such as Tor
std::chrono::milliseconds g_socks5_recv_timeout = 20s;
CThreadInterrupt g_socks5_interrupt;

ReachableNets g_reachable_nets;

std::vector<CNetAddr> WrappedGetAddrInfo(const std::string& name, bool allow_lookup)
{
    addrinfo ai_hint{};
    // We want a TCP port, which is a streaming socket type
    ai_hint.ai_socktype = SOCK_STREAM;
    ai_hint.ai_protocol = IPPROTO_TCP;
    // We don't care which address family (IPv4 or IPv6) is returned
    ai_hint.ai_family = AF_UNSPEC;

    // If we allow lookups of hostnames, use the AI_ADDRCONFIG flag to only
    // return addresses whose family we have an address configured for.
    //
    // If we don't allow lookups, then use the AI_NUMERICHOST flag for
    // getaddrinfo to only decode numerical network addresses and suppress
    // hostname lookups.
    ai_hint.ai_flags = allow_lookup ? AI_ADDRCONFIG : AI_NUMERICHOST;

    addrinfo* ai_res{nullptr};
    const int n_err{getaddrinfo(name.c_str(), nullptr, &ai_hint, &ai_res)};
    if (n_err != 0) {
        if ((ai_hint.ai_flags & AI_ADDRCONFIG) == AI_ADDRCONFIG) {
            // AI_ADDRCONFIG on some systems may exclude loopback-only addresses
            // If first lookup failed we perform a second lookup without AI_ADDRCONFIG
            ai_hint.ai_flags = (ai_hint.ai_flags & ~AI_ADDRCONFIG);
            const int n_err_retry{getaddrinfo(name.c_str(), nullptr, &ai_hint, &ai_res)};
            if (n_err_retry != 0) {
                return {};
            }
        } else {
            return {};
        }
    }

    // Traverse the linked list starting with ai_trav.
    addrinfo* ai_trav{ai_res};
    std::vector<CNetAddr> resolved_addresses;
    while (ai_trav != nullptr) {
        if (ai_trav->ai_family == AF_INET) {
            assert(ai_trav->ai_addrlen >= sizeof(sockaddr_in));
            resolved_addresses.emplace_back(reinterpret_cast<sockaddr_in*>(ai_trav->ai_addr)->sin_addr);
        }
        if (ai_trav->ai_family == AF_INET6) {
            assert(ai_trav->ai_addrlen >= sizeof(sockaddr_in6));
            const sockaddr_in6* s6{reinterpret_cast<sockaddr_in6*>(ai_trav->ai_addr)};
