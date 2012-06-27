// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_SCRIPT_SIGNINGPROVIDER_H
#define BITCOIN_SCRIPT_SIGNINGPROVIDER_H

#include <addresstype.h>
#include <attributes.h>
#include <key.h>
#include <pubkey.h>
#include <script/keyorigin.h>
#include <script/script.h>
#include <sync.h>

struct ShortestVectorFirstComparator
{
    bool operator()(const std::vector<unsigned char>& a, const std::vector<unsigned char>& b) const
    {
        if (a.size() < b.size()) return true;
        if (a.size() > b.size()) return false;
        return a < b;
    }
};

struct TaprootSpendData
{
    /** The BIP341 internal key. */
    XOnlyPubKey internal_key;
    /** The Merkle root of the script tree (0 if no scripts). */
