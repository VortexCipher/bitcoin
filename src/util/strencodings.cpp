// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <util/strencodings.h>

#include <crypto/hex_base.h>
#include <span.h>

#include <array>
#include <cassert>
#include <cstring>
#include <limits>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

static const std::string CHARS_ALPHA_NUM = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

static const std::string SAFE_CHARS[] =
{
    CHARS_ALPHA_NUM + " .,;-_/:?@()", // SAFE_CHARS_DEFAULT
    CHARS_ALPHA_NUM + " .,;-_?@", // SAFE_CHARS_UA_COMMENT
    CHARS_ALPHA_NUM + ".-_", // SAFE_CHARS_FILENAME
    CHARS_ALPHA_NUM + "!*'();:@&=+$,/?#[]-_.~%", // SAFE_CHARS_URI
};

std::string SanitizeString(std::string_view str, int rule)
{
    std::string result;
    for (char c : str) {
        if (SAFE_CHARS[rule].find(c) != std::string::npos) {
            result.push_back(c);
        }
    }
    return result;
}

bool IsHex(std::string_view str)
{
    for (char c : str) {
        if (HexDigit(c) < 0) return false;
    }
    return (str.size() > 0) && (str.size()%2 == 0);
}

bool IsHexNumber(std::string_view str)
