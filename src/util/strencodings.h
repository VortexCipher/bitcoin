// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

/**
 * Utilities for converting data from/to strings.
 */
#ifndef BITCOIN_UTIL_STRENCODINGS_H
#define BITCOIN_UTIL_STRENCODINGS_H

#include <crypto/hex_base.h> // IWYU pragma: export
#include <span.h>
#include <util/string.h>

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <string>      // IWYU pragma: export
#include <string_view> // IWYU pragma: export
#include <system_error>
#include <type_traits>
#include <vector>

/** Used by SanitizeString() */
enum SafeChars
{
    SAFE_CHARS_DEFAULT, //!< The full set of allowed chars
    SAFE_CHARS_UA_COMMENT, //!< BIP-0014 subset
    SAFE_CHARS_FILENAME, //!< Chars allowed in filenames
    SAFE_CHARS_URI, //!< Chars allowed in URIs (RFC 3986)
};

/**
 * Used by ParseByteUnits()
