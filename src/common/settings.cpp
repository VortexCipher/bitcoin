// Copyright (c) 2019-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <common/settings.h>

#include <config/bitcoin-config.h> // IWYU pragma: keep

#include <tinyformat.h>
#include <univalue.h>
#include <util/fs.h>

#include <algorithm>
#include <fstream>
#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace common {
namespace {

enum class Source {
   FORCED,
   COMMAND_LINE,
   RW_SETTINGS,
