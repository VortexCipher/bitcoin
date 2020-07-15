// Copyright (c) 2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <util/result.h>

#include <boost/test/unit_test.hpp>

inline bool operator==(const bilingual_str& a, const bilingual_str& b)
