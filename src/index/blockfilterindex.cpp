// Copyright (c) 2018-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <map>

#include <clientversion.h>
#include <common/args.h>
#include <dbwrapper.h>
#include <hash.h>
#include <index/blockfilterindex.h>
#include <logging.h>
#include <node/blockstorage.h>
#include <undo.h>
#include <util/fs_helpers.h>
#include <validation.h>

/* The index database stores three items for each block: the disk location of the encoded filter,
 * its dSHA256 hash, and the header. Those belonging to blocks on the active chain are indexed by
 * height, and those belonging to blocks that have been reorganized out of the active chain are
 * indexed by block hash. This ensures that filter data for any block that becomes part of the
 * active chain can always be retrieved, alleviating timing concerns.
 *
 * The filters themselves are stored in flat files and referenced by the LevelDB entries. This
 * minimizes the amount of data written to LevelDB and keeps the database values constant size. The
 * disk location of the next block filter to be written (represented as a FlatFilePos) is stored
 * under the DB_FILTER_POS key.
 *
 * Keys for the height index have the type [DB_BLOCK_HEIGHT, uint32 (BE)]. The height is represented
 * as big-endian so that sequential reads of filters by height are fast.
 * Keys for the hash index have the type [DB_BLOCK_HASH, uint256].
 */
constexpr uint8_t DB_BLOCK_HASH{'s'};
constexpr uint8_t DB_BLOCK_HEIGHT{'t'};
constexpr uint8_t DB_FILTER_POS{'P'};

constexpr unsigned int MAX_FLTR_FILE_SIZE = 0x1000000; // 16 MiB
/** The pre-allocation chunk size for fltr?????.dat files */
constexpr unsigned int FLTR_FILE_CHUNK_SIZE = 0x100000; // 1 MiB
/** Maximum size of the cfheaders cache
 *  We have a limit to prevent a bug in filling this cache
 *  potentially turning into an OOM. At 2000 entries, this cache
 *  is big enough for a 2,000,000 length block chain, which
 *  we should be enough until ~2047. */
constexpr size_t CF_HEADERS_CACHE_MAX_SZ{2000};

namespace {

struct DBVal {
    uint256 hash;
    uint256 header;
    FlatFilePos pos;
