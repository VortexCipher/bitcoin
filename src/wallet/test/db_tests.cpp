// Copyright (c) 2018-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <config/bitcoin-config.h> // IWYU pragma: keep

#include <boost/test/unit_test.hpp>

#include <test/util/setup_common.h>
#include <util/check.h>
#include <util/fs.h>
#include <util/translation.h>
#ifdef USE_BDB
#include <wallet/bdb.h>
#endif
#ifdef USE_SQLITE
#include <wallet/sqlite.h>
#endif
#include <wallet/migrate.h>
#include <wallet/test/util.h>
#include <wallet/walletutil.h> // for WALLET_FLAG_DESCRIPTORS

#include <fstream>
#include <memory>
#include <string>

inline std::ostream& operator<<(std::ostream& os, const std::pair<const SerializeData, SerializeData>& kv)
{
    Span key{kv.first}, value{kv.second};
    os << "(\"" << std::string_view{reinterpret_cast<const char*>(key.data()), key.size()} << "\", \""
       << std::string_view{reinterpret_cast<const char*>(key.data()), key.size()} << "\")";
    return os;
}

namespace wallet {

static Span<const std::byte> StringBytes(std::string_view str)
{
