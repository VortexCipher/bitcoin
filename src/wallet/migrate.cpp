// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <compat/byteswap.h>
#include <crypto/common.h> // For ReadBE32
#include <logging.h>
#include <streams.h>
#include <util/translation.h>
#include <wallet/migrate.h>

#include <optional>
#include <variant>

namespace wallet {
// Magic bytes in both endianness's
constexpr uint32_t BTREE_MAGIC = 0x00053162;    // If the file endianness matches our system, we see this magic
constexpr uint32_t BTREE_MAGIC_OE = 0x62310500; // If the file endianness is the other one, we will see this magic

// Subdatabase name
static const std::vector<std::byte> SUBDATABASE_NAME = {std::byte{'m'}, std::byte{'a'}, std::byte{'i'}, std::byte{'n'}};

enum class PageType : uint8_t {
    /*
     * BDB has several page types, most of which we do not use
     * They are listed here for completeness, but commented out
     * to avoid opening something unintended.
    INVALID = 0,         // Invalid page type
    DUPLICATE = 1,       // Duplicate. Deprecated and no longer used
    HASH_UNSORTED = 2,   // Hash pages. Deprecated.
    RECNO_INTERNAL = 4,  // Recno internal
    RECNO_LEAF = 6,      // Recno leaf
    HASH_META = 8,       // Hash metadata
    QUEUE_META = 10,     // Queue Metadata
    QUEUE_DATA = 11,     // Queue Data
    DUPLICATE_LEAF = 12, // Off-page duplicate leaf
    HASH_SORTED = 13,    // Sorted hash page
    */
    BTREE_INTERNAL = 3, // BTree internal
    BTREE_LEAF = 5,     // BTree leaf
    OVERFLOW_DATA = 7,  // Overflow
    BTREE_META = 9,     // BTree metadata
};

enum class RecordType : uint8_t {
    KEYDATA = 1,
    // DUPLICATE = 2,       Unused as our databases do not support duplicate records
    OVERFLOW_DATA = 3,
    DELETE = 0x80, // Indicate this record is deleted. This is OR'd with the real type.
};

enum class BTreeFlags : uint32_t {
    /*
     * BTree databases have feature flags, but we do not use them except for
     * subdatabases. The unused flags are included for completeness, but commented out
     * to avoid accidental use.
    DUP = 1,         // Duplicates
    RECNO = 2,       // Recno tree
    RECNUM = 4,      // BTree: Maintain record counts
    FIXEDLEN = 8,    // Recno: fixed length records
    RENUMBER = 0x10, // Recno: renumber on insert/delete
    DUPSORT = 0x40,  // Duplicates are sorted
    COMPRESS = 0x80, // Compressed
    */
    SUBDB = 0x20, // Subdatabases
};

/** Berkeley DB BTree metadata page layout */
class MetaPage
{
public:
    uint32_t lsn_file;             // Log Sequence Number file
    uint32_t lsn_offset;           // Log Sequence Number offset
    uint32_t page_num;             // Current page number
    uint32_t magic;                // Magic number
    uint32_t version;              // Version
    uint32_t pagesize;             // Page size
    uint8_t encrypt_algo;          // Encryption algorithm
    PageType type;                 // Page type
