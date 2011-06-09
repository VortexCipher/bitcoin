// Copyright (c) 2017-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_RPC_UTIL_H
#define BITCOIN_RPC_UTIL_H

#include <addresstype.h>
#include <consensus/amount.h>
#include <node/transaction.h>
#include <outputtype.h>
#include <pubkey.h>
#include <rpc/protocol.h>
#include <rpc/request.h>
#include <script/script.h>
#include <script/sign.h>
#include <uint256.h>
#include <univalue.h>
#include <util/check.h>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

class JSONRPCRequest;
enum ServiceFlags : uint64_t;
enum class OutputType;
struct FlatSigningProvider;
struct bilingual_str;
namespace common {
enum class PSBTError;
} // namespace common
namespace node {
enum class TransactionError;
} // namespace node

static constexpr bool DEFAULT_RPC_DOC_CHECK{
#ifdef RPC_DOC_CHECK
    true
#else
    false
