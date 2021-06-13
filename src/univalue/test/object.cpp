// Copyright (c) 2014 BitPay Inc.
// Copyright (c) 2014-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/licenses/mit-license.php.

#include <univalue.h>

#include <cassert>
#include <cstdint>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#define BOOST_CHECK(expr) assert(expr)
#define BOOST_CHECK_EQUAL(v1, v2) assert((v1) == (v2))
#define BOOST_CHECK_THROW(stmt, excMatch) { \
        try { \
            (stmt); \
            assert(0 && "No exception caught"); \
        } catch (excMatch&) { \
        } catch (...) { \
            assert(0 && "Wrong exception caught"); \
        } \
    }
#define BOOST_CHECK_NO_THROW(stmt) { \
        try { \
            (stmt); \
        } catch (...) { \
            assert(0); \
        } \
    }

void univalue_constructor()
{
    UniValue v1;
    BOOST_CHECK(v1.isNull());

    UniValue v2(UniValue::VSTR);
    BOOST_CHECK(v2.isStr());

    UniValue v3(UniValue::VSTR, "foo");
    BOOST_CHECK(v3.isStr());
    BOOST_CHECK_EQUAL(v3.getValStr(), "foo");
