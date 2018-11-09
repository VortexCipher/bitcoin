// Copyright 2014 BitPay Inc.
// Copyright 2015 Bitcoin Core Developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/licenses/mit-license.php.

#include <univalue.h>

#include <iomanip>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

const UniValue NullUniValue;

void UniValue::clear()
{
    typ = VNULL;
    val.clear();
    keys.clear();
    values.clear();
}
