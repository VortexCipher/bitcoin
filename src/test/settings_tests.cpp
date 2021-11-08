// Copyright (c) 2011-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <common/settings.h>

#include <test/util/setup_common.h>
#include <test/util/str.h>

#include <boost/test/unit_test.hpp>
#include <common/args.h>
#include <univalue.h>
#include <util/chaintype.h>
#include <util/fs.h>
#include <util/strencodings.h>
#include <util/string.h>

#include <fstream>
#include <map>
#include <string>
#include <system_error>
#include <vector>

using util::ToString;

inline bool operator==(const common::SettingsValue& a, const common::SettingsValue& b)
