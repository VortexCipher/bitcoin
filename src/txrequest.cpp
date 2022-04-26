// Copyright (c) 2020-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <txrequest.h>

#include <crypto/siphash.h>
#include <net.h>
#include <primitives/transaction.h>
#include <random.h>
#include <uint256.h>

#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/tag.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/tuple/tuple.hpp>

#include <chrono>
#include <unordered_map>
#include <utility>

#include <assert.h>

namespace {

/** The various states a (txhash,peer) pair can be in.
 *
 * Note that CANDIDATE is split up into 3 substates (DELAYED, BEST, READY), allowing more efficient implementation.
 * Also note that the sorting order of ByTxHashView relies on the specific order of values in this enum.
 *
 * Expected behaviour is:
 *   - When first announced by a peer, the state is CANDIDATE_DELAYED until reqtime is reached.
 *   - Announcements that have reached their reqtime but not been requested will be either CANDIDATE_READY or
 *     CANDIDATE_BEST. Neither of those has an expiration time; they remain in that state until they're requested or
 *     no longer needed. CANDIDATE_READY announcements are promoted to CANDIDATE_BEST when they're the best one left.
 *   - When requested, an announcement will be in state REQUESTED until expiry is reached.
 *   - If expiry is reached, or the peer replies to the request (either with NOTFOUND or the tx), the state becomes
 *     COMPLETED.
 */
enum class State : uint8_t {
    /** A CANDIDATE announcement whose reqtime is in the future. */
    CANDIDATE_DELAYED,
    /** A CANDIDATE announcement that's not CANDIDATE_DELAYED or CANDIDATE_BEST. */
    CANDIDATE_READY,
    /** The best CANDIDATE for a given txhash; only if there is no REQUESTED announcement already for that txhash.
     *  The CANDIDATE_BEST is the highest-priority announcement among all CANDIDATE_READY (and _BEST) ones for that
     *  txhash. */
    CANDIDATE_BEST,
    /** A REQUESTED announcement. */
    REQUESTED,
    /** A COMPLETED announcement. */
    COMPLETED,
};

//! Type alias for sequence numbers.
using SequenceNumber = uint64_t;

/** An announcement. This is the data we track for each txid or wtxid that is announced to us by each peer. */
struct Announcement {
    /** Txid or wtxid that was announced. */
    const uint256 m_txhash;
    /** For CANDIDATE_{DELAYED,BEST,READY} the reqtime; for REQUESTED the expiry. */
    std::chrono::microseconds m_time;
    /** What peer the request was from. */
    const NodeId m_peer;
    /** What sequence number this announcement has. */
    const SequenceNumber m_sequence : 59;
    /** Whether the request is preferred. */
    const bool m_preferred : 1;
    /** Whether this is a wtxid request. */
    const bool m_is_wtxid : 1;

    /** What state this announcement is in. */
