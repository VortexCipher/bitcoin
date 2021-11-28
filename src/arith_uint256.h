// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_ARITH_UINT256_H
#define BITCOIN_ARITH_UINT256_H

#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>

class uint256;

class uint_error : public std::runtime_error {
public:
    explicit uint_error(const std::string& str) : std::runtime_error(str) {}
};

/** Template base class for unsigned big integers. */
template<unsigned int BITS>
class base_uint
{
protected:
    static_assert(BITS / 32 > 0 && BITS % 32 == 0, "Template parameter BITS must be a positive multiple of 32.");
    static constexpr int WIDTH = BITS / 32;
