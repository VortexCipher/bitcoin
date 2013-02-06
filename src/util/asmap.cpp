// Copyright (c) 2019-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <util/asmap.h>

#include <clientversion.h>
#include <logging.h>
#include <serialize.h>
#include <streams.h>
#include <util/fs.h>

#include <algorithm>
#include <bit>
#include <cassert>
#include <cstdio>
#include <utility>
#include <vector>

namespace {

constexpr uint32_t INVALID = 0xFFFFFFFF;
