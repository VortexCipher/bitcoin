// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_HASH_H
#define BITCOIN_HASH_H

#include <attributes.h>
#include <crypto/common.h>
#include <crypto/ripemd160.h>
#include <crypto/sha256.h>
#include <prevector.h>
#include <serialize.h>
#include <span.h>
#include <uint256.h>

#include <string>
#include <vector>

typedef uint256 ChainCode;

