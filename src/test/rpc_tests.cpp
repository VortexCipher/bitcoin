// Copyright (c) 2012-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <core_io.h>
#include <interfaces/chain.h>
#include <node/context.h>
#include <rpc/blockchain.h>
#include <rpc/client.h>
#include <rpc/server.h>
#include <rpc/util.h>
#include <test/util/setup_common.h>
#include <univalue.h>
#include <util/time.h>

#include <any>

#include <boost/test/unit_test.hpp>

using util::SplitString;

static UniValue JSON(std::string_view json)
{
    UniValue value;
    BOOST_CHECK(value.read(json));
    return value;
}

class HasJSON
{
public:
    explicit HasJSON(std::string json) : m_json(std::move(json)) {}
    bool operator()(const UniValue& value) const
    {
        std::string json{value.write()};
        BOOST_CHECK_EQUAL(json, m_json);
        return json == m_json;
    };

private:
    const std::string m_json;
};

class RPCTestingSetup : public TestingSetup
{
public:
    UniValue TransformParams(const UniValue& params, std::vector<std::pair<std::string, bool>> arg_names) const;
    UniValue CallRPC(std::string args);
};

UniValue RPCTestingSetup::TransformParams(const UniValue& params, std::vector<std::pair<std::string, bool>> arg_names) const
{
    UniValue transformed_params;
    CRPCTable table;
    CRPCCommand command{"category", "method", [&](const JSONRPCRequest& request, UniValue&, bool) -> bool { transformed_params = request.params; return true; }, arg_names, /*unique_id=*/0};
    table.appendCommand("method", &command);
    JSONRPCRequest request;
    request.strMethod = "method";
    request.params = params;
    if (RPCIsInWarmup(nullptr)) SetRPCWarmupFinished();
    table.execute(request);
    return transformed_params;
}

