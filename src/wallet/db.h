// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_WALLET_DB_H
#define BITCOIN_WALLET_DB_H

#include <clientversion.h>
#include <streams.h>
#include <support/allocators/secure.h>
#include <util/fs.h>

#include <atomic>
#include <memory>
#include <optional>
#include <string>

class ArgsManager;
struct bilingual_str;

namespace wallet {
// BytePrefix compares equality with other byte spans that begin with the same prefix.
