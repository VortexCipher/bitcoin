// Copyright (c) 2017-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_UTIL_FS_H
#define BITCOIN_UTIL_FS_H

#include <tinyformat.h>

#include <cstdio>
#include <filesystem> // IWYU pragma: export
#include <functional>
#include <iomanip>
#include <ios>
#include <ostream>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>

/** Filesystem operations and types */
namespace fs {

using namespace std::filesystem;

