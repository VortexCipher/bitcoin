// Copyright (c) 2012-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <hash.h>
#include <serialize.h>
#include <streams.h>
#include <test/util/setup_common.h>
#include <util/strencodings.h>

#include <stdint.h>
#include <string>

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(serialize_tests, BasicTestingSetup)

// For testing move-semantics, declare a version of datastream that can be moved
// but is not copyable.
class UncopyableStream : public DataStream
{
public:
    using DataStream::DataStream;
    UncopyableStream(const UncopyableStream&) = delete;
    UncopyableStream& operator=(const UncopyableStream&) = delete;
    UncopyableStream(UncopyableStream&&) noexcept = default;
    UncopyableStream& operator=(UncopyableStream&&) noexcept = default;
};

class CSerializeMethodsTestSingle
{
protected:
    int intval;
    bool boolval;
    std::string stringval;
    char charstrval[16];
    CTransactionRef txval;
public:
    CSerializeMethodsTestSingle() = default;
    CSerializeMethodsTestSingle(int intvalin, bool boolvalin, std::string stringvalin, const uint8_t* charstrvalin, const CTransactionRef& txvalin) : intval(intvalin), boolval(boolvalin), stringval(std::move(stringvalin)), txval(txvalin)
    {
        memcpy(charstrval, charstrvalin, sizeof(charstrval));
    }

    SERIALIZE_METHODS(CSerializeMethodsTestSingle, obj)
    {
        READWRITE(obj.intval);
        READWRITE(obj.boolval);
        READWRITE(obj.stringval);
