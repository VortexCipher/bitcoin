// Copyright (c) 2023 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_COMMON_ARGS_H
#define BITCOIN_COMMON_ARGS_H

#include <common/settings.h>
#include <compat/compat.h>
#include <sync.h>
#include <util/chaintype.h>
#include <util/fs.h>

#include <iosfwd>
#include <list>
#include <map>
#include <optional>
#include <set>
#include <stdint.h>
#include <string>
#include <variant>
#include <vector>

class ArgsManager;

extern const char * const BITCOIN_CONF_FILENAME;
extern const char * const BITCOIN_SETTINGS_FILENAME;

// Return true if -datadir option points to a valid directory or is not specified.
bool CheckDataDirOption(const ArgsManager& args);

/**
 * Most paths passed as configuration arguments are treated as relative to
 * the datadir if they are not absolute.
 *
 * @param args Parsed arguments and settings.
 * @param path The path to be conditionally prefixed with datadir.
 * @param net_specific Use network specific datadir variant
 * @return The normalized path.
 */
fs::path AbsPathForConfigVal(const ArgsManager& args, const fs::path& path, bool net_specific = true);

inline bool IsSwitchChar(char c)
{
#ifdef WIN32
    return c == '-' || c == '/';
#else
    return c == '-';
