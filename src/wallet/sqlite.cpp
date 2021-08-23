// Copyright (c) 2020-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <config/bitcoin-config.h> // IWYU pragma: keep

#include <wallet/sqlite.h>

#include <chainparams.h>
#include <crypto/common.h>
#include <logging.h>
#include <sync.h>
#include <util/fs_helpers.h>
#include <util/check.h>
#include <util/strencodings.h>
#include <util/translation.h>
#include <wallet/db.h>

#include <sqlite3.h>
#include <stdint.h>

#include <optional>
#include <utility>
#include <vector>

namespace wallet {
static constexpr int32_t WALLET_SCHEMA_VERSION = 0;

static Span<const std::byte> SpanFromBlob(sqlite3_stmt* stmt, int col)
{
    return {reinterpret_cast<const std::byte*>(sqlite3_column_blob(stmt, col)),
            static_cast<size_t>(sqlite3_column_bytes(stmt, col))};
}

static void ErrorLogCallback(void* arg, int code, const char* msg)
{
    // From sqlite3_config() documentation for the SQLITE_CONFIG_LOG option:
    // "The void pointer that is the second argument to SQLITE_CONFIG_LOG is passed through as
    // the first parameter to the application-defined logger function whenever that function is
    // invoked."
    // Assert that this is the case:
    assert(arg == nullptr);
    LogPrintf("SQLite Error. Code: %d. Message: %s\n", code, msg);
}

static int TraceSqlCallback(unsigned code, void* context, void* param1, void* param2)
{
    auto* db = static_cast<SQLiteDatabase*>(context);
    if (code == SQLITE_TRACE_STMT) {
        auto* stmt = static_cast<sqlite3_stmt*>(param1);
        // To be conservative and avoid leaking potentially secret information
        // in the log file, only expand statements that query the database, not
        // statements that update the database.
        char* expanded{sqlite3_stmt_readonly(stmt) ? sqlite3_expanded_sql(stmt) : nullptr};
        LogTrace(BCLog::WALLETDB, "[%s] SQLite Statement: %s\n", db->Filename(), expanded ? expanded : sqlite3_sql(stmt));
        if (expanded) sqlite3_free(expanded);
    }
    return SQLITE_OK;
}

static bool BindBlobToStatement(sqlite3_stmt* stmt,
                                int index,
                                Span<const std::byte> blob,
                                const std::string& description)
{
    // Pass a pointer to the empty string "" below instead of passing the
    // blob.data() pointer if the blob.data() pointer is null. Passing a null
    // data pointer to bind_blob would cause sqlite to bind the SQL NULL value
    // instead of the empty blob value X'', which would mess up SQL comparisons.
    int res = sqlite3_bind_blob(stmt, index, blob.data() ? static_cast<const void*>(blob.data()) : "", blob.size(), SQLITE_STATIC);
    if (res != SQLITE_OK) {
