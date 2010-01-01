// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_WALLET_BDB_H
#define BITCOIN_WALLET_BDB_H

#include <clientversion.h>
#include <common/system.h>
#include <serialize.h>
#include <streams.h>
#include <util/fs.h>
#include <wallet/db.h>

#include <atomic>
#include <condition_variable>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
