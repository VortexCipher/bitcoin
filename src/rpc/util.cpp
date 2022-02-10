// Copyright (c) 2017-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <config/bitcoin-config.h> // IWYU pragma: keep

#include <clientversion.h>
#include <common/args.h>
#include <common/messages.h>
#include <common/types.h>
#include <consensus/amount.h>
#include <core_io.h>
#include <key_io.h>
#include <node/types.h>
#include <outputtype.h>
#include <rpc/util.h>
#include <script/descriptor.h>
#include <script/interpreter.h>
#include <script/signingprovider.h>
#include <script/solver.h>
#include <tinyformat.h>
#include <univalue.h>
#include <util/check.h>
#include <util/result.h>
#include <util/strencodings.h>
#include <util/string.h>
#include <util/translation.h>

#include <algorithm>
#include <iterator>
#include <string_view>
#include <tuple>
#include <utility>

using common::PSBTError;
using common::PSBTErrorString;
using common::TransactionErrorString;
using node::TransactionError;
using util::Join;
using util::SplitString;
using util::TrimString;

const std::string UNIX_EPOCH_TIME = "UNIX epoch time";
const std::string EXAMPLE_ADDRESS[2] = {"bc1q09vm5lfy0j5reeulh4x5752q25uqqvz34hufdl", "bc1q02ad21edsxd23d32dfgqqsz4vv4nmtfzuklhy3"};

std::string GetAllOutputTypes()
{
    std::vector<std::string> ret;
    using U = std::underlying_type<TxoutType>::type;
    for (U i = (U)TxoutType::NONSTANDARD; i <= (U)TxoutType::WITNESS_UNKNOWN; ++i) {
        ret.emplace_back(GetTxnOutputType(static_cast<TxoutType>(i)));
    }
    return Join(ret, ", ");
}

void RPCTypeCheckObj(const UniValue& o,
    const std::map<std::string, UniValueType>& typesExpected,
    bool fAllowNull,
    bool fStrict)
{
    for (const auto& t : typesExpected) {
        const UniValue& v = o.find_value(t.first);
        if (!fAllowNull && v.isNull())
            throw JSONRPCError(RPC_TYPE_ERROR, strprintf("Missing %s", t.first));

        if (!(t.second.typeAny || v.type() == t.second.type || (fAllowNull && v.isNull())))
            throw JSONRPCError(RPC_TYPE_ERROR, strprintf("JSON value of type %s for field %s is not of expected type %s", uvTypeName(v.type()),  t.first, uvTypeName(t.second.type)));
    }

    if (fStrict)
    {
        for (const std::string& k : o.getKeys())
        {
            if (typesExpected.count(k) == 0)
            {
                std::string err = strprintf("Unexpected key %s", k);
                throw JSONRPCError(RPC_TYPE_ERROR, err);
            }
        }
    }
}

CAmount AmountFromValue(const UniValue& value, int decimals)
{
    if (!value.isNum() && !value.isStr())
        throw JSONRPCError(RPC_TYPE_ERROR, "Amount is not a number or string");
    CAmount amount;
    if (!ParseFixedPoint(value.getValStr(), decimals, &amount))
        throw JSONRPCError(RPC_TYPE_ERROR, "Invalid amount");
    if (!MoneyRange(amount))
        throw JSONRPCError(RPC_TYPE_ERROR, "Amount out of range");
    return amount;
}

CFeeRate ParseFeeRate(const UniValue& json)
{
    CAmount val{AmountFromValue(json)};
    if (val >= COIN) throw JSONRPCError(RPC_INVALID_PARAMETER, "Fee rates larger than or equal to 1BTC/kvB are not accepted");
    return CFeeRate{val};
}

uint256 ParseHashV(const UniValue& v, std::string_view name)
{
    const std::string& strHex(v.get_str());
    if (64 != strHex.length())
        throw JSONRPCError(RPC_INVALID_PARAMETER, strprintf("%s must be of length %d (not %d, for '%s')", name, 64, strHex.length(), strHex));
    if (!IsHex(strHex)) // Note: IsHex("") is false
        throw JSONRPCError(RPC_INVALID_PARAMETER, strprintf("%s must be hexadecimal string (not '%s')", name, strHex));
    return uint256S(strHex);
}
uint256 ParseHashO(const UniValue& o, std::string_view strKey)
{
    return ParseHashV(o.find_value(strKey), strKey);
}
std::vector<unsigned char> ParseHexV(const UniValue& v, std::string_view name)
{
    std::string strHex;
    if (v.isStr())
        strHex = v.get_str();
    if (!IsHex(strHex))
        throw JSONRPCError(RPC_INVALID_PARAMETER, strprintf("%s must be hexadecimal string (not '%s')", name, strHex));
    return ParseHex(strHex);
}
std::vector<unsigned char> ParseHexO(const UniValue& o, std::string_view strKey)
{
    return ParseHexV(o.find_value(strKey), strKey);
}

namespace {

/**
 * Quote an argument for shell.
 *
 * @note This is intended for help, not for security-sensitive purposes.
 */
std::string ShellQuote(const std::string& s)
{
    std::string result;
    result.reserve(s.size() * 2);
