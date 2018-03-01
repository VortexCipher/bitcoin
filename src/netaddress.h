// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_NETADDRESS_H
#define BITCOIN_NETADDRESS_H

#include <compat/compat.h>
#include <crypto/siphash.h>
#include <prevector.h>
#include <random.h>
#include <serialize.h>
#include <tinyformat.h>
#include <util/strencodings.h>
#include <util/string.h>

#include <array>
#include <cstdint>
#include <ios>
#include <string>
#include <vector>

/**
 * A network type.
 * @note An address may belong to more than one network, for example `10.0.0.1`
 * belongs to both `NET_UNROUTABLE` and `NET_IPV4`.
 * Keep these sequential starting from 0 and `NET_MAX` as the last entry.
 * We have loops like `for (int i = 0; i < NET_MAX; ++i)` that expect to iterate
 * over all enum values and also `GetExtNetwork()` "extends" this enum by
 * introducing standalone constants starting from `NET_MAX`.
 */
enum Network {
    /// Addresses from these networks are not publicly routable on the global Internet.
    NET_UNROUTABLE = 0,

    /// IPv4
    NET_IPV4,

    /// IPv6
    NET_IPV6,

    /// TOR (v2 or v3)
    NET_ONION,

    /// I2P
    NET_I2P,

    /// CJDNS
    NET_CJDNS,

    /// A set of addresses that represent the hash of a string or FQDN. We use
    /// them in AddrMan to keep track of which DNS seeds were used.
    NET_INTERNAL,

    /// Dummy value to indicate the number of NET_* constants.
    NET_MAX,
};

/// Prefix of an IPv6 address when it contains an embedded IPv4 address.
