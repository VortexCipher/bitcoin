// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_VALIDATION_H
#define BITCOIN_VALIDATION_H

#include <arith_uint256.h>
#include <attributes.h>
#include <chain.h>
#include <checkqueue.h>
#include <consensus/amount.h>
#include <cuckoocache.h>
#include <deploymentstatus.h>
#include <kernel/chain.h>
#include <kernel/chainparams.h>
#include <kernel/chainstatemanager_opts.h>
#include <kernel/cs_main.h> // IWYU pragma: export
#include <node/blockstorage.h>
#include <policy/feerate.h>
#include <policy/packages.h>
#include <policy/policy.h>
#include <script/script_error.h>
#include <script/sigcache.h>
#include <sync.h>
#include <txdb.h>
#include <txmempool.h> // For CTxMemPool::cs
#include <uint256.h>
#include <util/check.h>
#include <util/fs.h>
#include <util/hasher.h>
#include <util/result.h>
#include <util/translation.h>
#include <versionbits.h>

#include <atomic>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <stdint.h>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

class Chainstate;
class CTxMemPool;
class ChainstateManager;
struct ChainTxData;
class DisconnectedBlockTransactions;
struct PrecomputedTransactionData;
struct LockPoints;
struct AssumeutxoData;
namespace node {
class SnapshotMetadata;
} // namespace node
namespace Consensus {
struct Params;
} // namespace Consensus
namespace util {
class SignalInterrupt;
} // namespace util

/** Block files containing a block-height within MIN_BLOCKS_TO_KEEP of ActiveChain().Tip() will not be pruned. */
static const unsigned int MIN_BLOCKS_TO_KEEP = 288;
static const signed int DEFAULT_CHECKBLOCKS = 6;
static constexpr int DEFAULT_CHECKLEVEL{3};
// Require that user allocate at least 550 MiB for block & undo files (blk???.dat and rev???.dat)
// At 1MB per block, 288 blocks = 288MB.
// Add 15% for Undo data = 331MB
// Add 20% for Orphan block rate = 397MB
// We want the low water mark after pruning to be at least 397 MB and since we prune in
// full block file chunks, we need the high water mark which triggers the prune to be
// one 128MB block file + added 15% undo data = 147MB greater for a total of 545MB
// Setting the target to >= 550 MiB will make it likely we can respect the target.
static const uint64_t MIN_DISK_SPACE_FOR_BLOCK_FILES = 550 * 1024 * 1024;

/** Current sync state passed to tip changed callbacks. */
enum class SynchronizationState {
    INIT_REINDEX,
    INIT_DOWNLOAD,
    POST_INIT
};

extern GlobalMutex g_best_block_mutex;
extern std::condition_variable g_best_block_cv;
/** Used to notify getblocktemplate RPC of new tips. */
extern uint256 g_best_block;

/** Documentation for argument 'checklevel'. */
extern const std::vector<std::string> CHECKLEVEL_DOC;

CAmount GetBlockSubsidy(int nHeight, const Consensus::Params& consensusParams);

bool FatalError(kernel::Notifications& notifications, BlockValidationState& state, const bilingual_str& message);

/** Guess verification progress (as a fraction between 0.0=genesis and 1.0=current tip). */
double GuessVerificationProgress(const ChainTxData& data, const CBlockIndex* pindex);

/** Prune block files up to a given height */
void PruneBlockFilesManual(Chainstate& active_chainstate, int nManualPruneHeight);

/**
* Validation result for a transaction evaluated by MemPoolAccept (single or package).
* Here are the expected fields and properties of a result depending on its ResultType, applicable to
* results returned from package evaluation:
*+---------------------------+----------------+-------------------+------------------+----------------+-------------------+
*| Field or property         |    VALID       |                 INVALID              |  MEMPOOL_ENTRY | DIFFERENT_WITNESS |
*|                           |                |--------------------------------------|                |                   |
*|                           |                | TX_RECONSIDERABLE |     Other        |                |                   |
*+---------------------------+----------------+-------------------+------------------+----------------+-------------------+
*| txid in mempool?          | yes            | no                | no*              | yes            | yes               |
*| wtxid in mempool?         | yes            | no                | no*              | yes            | no                |
*| m_state                   | yes, IsValid() | yes, IsInvalid()  | yes, IsInvalid() | yes, IsValid() | yes, IsValid()    |
*| m_vsize                   | yes            | no                | no               | yes            | no                |
*| m_base_fees               | yes            | no                | no               | yes            | no                |
*| m_effective_feerate       | yes            | yes               | no               | no             | no                |
*| m_wtxids_fee_calculations | yes            | yes               | no               | no             | no                |
*| m_other_wtxid             | no             | no                | no               | no             | yes               |
*+---------------------------+----------------+-------------------+------------------+----------------+-------------------+
* (*) Individual transaction acceptance doesn't return MEMPOOL_ENTRY and DIFFERENT_WITNESS. It returns
* INVALID, with the errors txn-already-in-mempool and txn-same-nonwitness-data-in-mempool
* respectively. In those cases, the txid or wtxid may be in the mempool for a TX_CONFLICT.
*/
struct MempoolAcceptResult {
    /** Used to indicate the results of mempool validation. */
    enum class ResultType {
        VALID, //!> Fully validated, valid.
        INVALID, //!> Invalid.
        MEMPOOL_ENTRY, //!> Valid, transaction was already in the mempool.
        DIFFERENT_WITNESS, //!> Not validated. A same-txid-different-witness tx (see m_other_wtxid) already exists in the mempool and was not replaced.
    };
