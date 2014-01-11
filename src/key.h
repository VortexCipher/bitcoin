// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Copyright (c) 2017 The Zcash developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_KEY_H
#define BITCOIN_KEY_H

#include <pubkey.h>
#include <serialize.h>
#include <support/allocators/secure.h>
#include <uint256.h>

#include <stdexcept>
#include <vector>


/**
 * CPrivKey is a serialized private key, with all parameters included
 * (SIZE bytes)
 */
typedef std::vector<unsigned char, secure_allocator<unsigned char> > CPrivKey;

/** Size of ECDH shared secrets. */
constexpr static size_t ECDH_SECRET_SIZE = CSHA256::OUTPUT_SIZE;

// Used to represent ECDH shared secret (ECDH_SECRET_SIZE bytes)
using ECDHSecret = std::array<std::byte, ECDH_SECRET_SIZE>;

class KeyPair;

