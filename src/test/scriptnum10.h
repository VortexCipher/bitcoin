// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_TEST_SCRIPTNUM10_H
#define BITCOIN_TEST_SCRIPTNUM10_H

#include <assert.h>
#include <limits>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <vector>

class scriptnum10_error : public std::runtime_error
{
public:
