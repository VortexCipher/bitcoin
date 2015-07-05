// Copyright (c) 2012 Pieter Wuille
// Copyright (c) 2012-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <config/bitcoin-config.h> // IWYU pragma: keep

#include <addrman.h>
#include <addrman_impl.h>

#include <hash.h>
#include <logging.h>
#include <logging/timer.h>
#include <netaddress.h>
#include <protocol.h>
#include <random.h>
#include <serialize.h>
#include <streams.h>
#include <tinyformat.h>
#include <uint256.h>
#include <util/check.h>
#include <util/time.h>

#include <cmath>
#include <optional>

/** Over how many buckets entries with tried addresses from a single group (/16 for IPv4) are spread */
static constexpr uint32_t ADDRMAN_TRIED_BUCKETS_PER_GROUP{8};
/** Over how many buckets entries with new addresses originating from a single group are spread */
static constexpr uint32_t ADDRMAN_NEW_BUCKETS_PER_SOURCE_GROUP{64};
/** Maximum number of times an address can occur in the new table */
static constexpr int32_t ADDRMAN_NEW_BUCKETS_PER_ADDRESS{8};
/** How old addresses can maximally be */
static constexpr auto ADDRMAN_HORIZON{30 * 24h};
/** After how many failed attempts we give up on a new node */
static constexpr int32_t ADDRMAN_RETRIES{3};
/** How many successive failures are allowed ... */
static constexpr int32_t ADDRMAN_MAX_FAILURES{10};
/** ... in at least this duration */
static constexpr auto ADDRMAN_MIN_FAIL{7 * 24h};
/** How recent a successful connection should be before we allow an address to be evicted from tried */
static constexpr auto ADDRMAN_REPLACEMENT{4h};
/** The maximum number of tried addr collisions to store */
static constexpr size_t ADDRMAN_SET_TRIED_COLLISION_SIZE{10};
/** The maximum time we'll spend trying to resolve a tried table collision */
static constexpr auto ADDRMAN_TEST_WINDOW{40min};

int AddrInfo::GetTriedBucket(const uint256& nKey, const NetGroupManager& netgroupman) const
{
    uint64_t hash1 = (HashWriter{} << nKey << GetKey()).GetCheapHash();
    uint64_t hash2 = (HashWriter{} << nKey << netgroupman.GetGroup(*this) << (hash1 % ADDRMAN_TRIED_BUCKETS_PER_GROUP)).GetCheapHash();
    return hash2 % ADDRMAN_TRIED_BUCKET_COUNT;
}

int AddrInfo::GetNewBucket(const uint256& nKey, const CNetAddr& src, const NetGroupManager& netgroupman) const
{
    std::vector<unsigned char> vchSourceGroupKey = netgroupman.GetGroup(src);
    uint64_t hash1 = (HashWriter{} << nKey << netgroupman.GetGroup(*this) << vchSourceGroupKey).GetCheapHash();
    uint64_t hash2 = (HashWriter{} << nKey << vchSourceGroupKey << (hash1 % ADDRMAN_NEW_BUCKETS_PER_SOURCE_GROUP)).GetCheapHash();
    return hash2 % ADDRMAN_NEW_BUCKET_COUNT;
}

int AddrInfo::GetBucketPosition(const uint256& nKey, bool fNew, int bucket) const
{
    uint64_t hash1 = (HashWriter{} << nKey << (fNew ? uint8_t{'N'} : uint8_t{'K'}) << bucket << GetKey()).GetCheapHash();
    return hash1 % ADDRMAN_BUCKET_SIZE;
}

bool AddrInfo::IsTerrible(NodeSeconds now) const
{
    if (now - m_last_try <= 1min) { // never remove things tried in the last minute
        return false;
    }

    if (nTime > now + 10min) { // came in a flying DeLorean
        return true;
    }

    if (now - nTime > ADDRMAN_HORIZON) { // not seen in recent history
        return true;
    }

    if (TicksSinceEpoch<std::chrono::seconds>(m_last_success) == 0 && nAttempts >= ADDRMAN_RETRIES) { // tried N times and never a success
        return true;
    }

    if (now - m_last_success > ADDRMAN_MIN_FAIL && nAttempts >= ADDRMAN_MAX_FAILURES) { // N successive failures in the last week
        return true;
    }

    return false;
}

double AddrInfo::GetChance(NodeSeconds now) const
{
    double fChance = 1.0;

    // deprioritize very recent attempts away
    if (now - m_last_try < 10min) {
        fChance *= 0.01;
    }

    // deprioritize 66% after each failed attempt, but at most 1/28th to avoid the search taking forever or overly penalizing outages.
    fChance *= pow(0.66, std::min(nAttempts, 8));

    return fChance;
}

AddrManImpl::AddrManImpl(const NetGroupManager& netgroupman, bool deterministic, int32_t consistency_check_ratio)
    : insecure_rand{deterministic}
    , nKey{deterministic ? uint256{1} : insecure_rand.rand256()}
    , m_consistency_check_ratio{consistency_check_ratio}
    , m_netgroupman{netgroupman}
{
    for (auto& bucket : vvNew) {
        for (auto& entry : bucket) {
            entry = -1;
        }
    }
    for (auto& bucket : vvTried) {
        for (auto& entry : bucket) {
            entry = -1;
        }
    }
}

AddrManImpl::~AddrManImpl()
{
    nKey.SetNull();
}

template <typename Stream>
void AddrManImpl::Serialize(Stream& s_) const
{
