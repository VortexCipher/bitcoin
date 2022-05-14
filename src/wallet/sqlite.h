// Copyright (c) 2020-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_WALLET_SQLITE_H
#define BITCOIN_WALLET_SQLITE_H

#include <sync.h>
#include <wallet/db.h>

struct bilingual_str;

struct sqlite3_stmt;
struct sqlite3;

namespace wallet {
class SQLiteDatabase;

