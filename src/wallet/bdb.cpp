// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <compat/compat.h>
#include <logging.h>
#include <util/fs.h>
#include <util/time.h>
#include <wallet/bdb.h>
#include <wallet/db.h>

#include <sync.h>
#include <util/check.h>
#include <util/fs_helpers.h>
#include <util/strencodings.h>
#include <util/translation.h>

#include <stdint.h>

#include <db_cxx.h>
#include <sys/stat.h>

// Windows may not define S_IRUSR or S_IWUSR. We define both
// here, with the same values as glibc (see stat.h).
#ifdef WIN32
#ifndef S_IRUSR
#define S_IRUSR             0400
#define S_IWUSR             0200
#endif
#endif

static_assert(BDB_DB_FILE_ID_LEN == DB_FILE_ID_LEN, "DB_FILE_ID_LEN should be 20.");

namespace wallet {
namespace {

//! Make sure database has a unique fileid within the environment. If it
//! doesn't, throw an error. BDB caches do not work properly when more than one
//! open database has the same fileid (values written to one database may show
//! up in reads to other databases).
//!
//! BerkeleyDB generates unique fileids by default
//! (https://docs.oracle.com/cd/E17275_01/html/programmer_reference/program_copy.html),
//! so bitcoin should never create different databases with the same fileid, but
//! this error can be triggered if users manually copy database files.
void CheckUniqueFileid(const BerkeleyEnvironment& env, const std::string& filename, Db& db, WalletDatabaseFileId& fileid)
{
    if (env.IsMock()) return;

    int ret = db.get_mpf()->get_fileid(fileid.value);
    if (ret != 0) {
        throw std::runtime_error(strprintf("BerkeleyDatabase: Can't open database %s (get_fileid failed with %d)", filename, ret));
    }

    for (const auto& item : env.m_fileids) {
        if (fileid == item.second && &fileid != &item.second) {
            throw std::runtime_error(strprintf("BerkeleyDatabase: Can't open database %s (duplicates fileid %s from %s)", filename,
                HexStr(item.second.value), item.first));
        }
    }
}

RecursiveMutex cs_db;
std::map<std::string, std::weak_ptr<BerkeleyEnvironment>> g_dbenvs GUARDED_BY(cs_db); //!< Map from directory name to db environment.
} // namespace

static constexpr auto REVERSE_BYTE_ORDER{std::endian::native == std::endian::little ? 4321 : 1234};

bool WalletDatabaseFileId::operator==(const WalletDatabaseFileId& rhs) const
{
    return memcmp(value, &rhs.value, sizeof(value)) == 0;
}

/**
 * @param[in] env_directory Path to environment directory
 * @return A shared pointer to the BerkeleyEnvironment object for the wallet directory, never empty because ~BerkeleyEnvironment
 * erases the weak pointer from the g_dbenvs map.
 * @post A new BerkeleyEnvironment weak pointer is inserted into g_dbenvs if the directory path key was not already in the map.
 */
std::shared_ptr<BerkeleyEnvironment> GetBerkeleyEnv(const fs::path& env_directory, bool use_shared_memory)
{
    LOCK(cs_db);
    auto inserted = g_dbenvs.emplace(fs::PathToString(env_directory), std::weak_ptr<BerkeleyEnvironment>());
    if (inserted.second) {
        auto env = std::make_shared<BerkeleyEnvironment>(env_directory, use_shared_memory);
        inserted.first->second = env;
        return env;
    }
    return inserted.first->second.lock();
}

//
// BerkeleyBatch
//

void BerkeleyEnvironment::Close()
