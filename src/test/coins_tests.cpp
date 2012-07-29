// Copyright (c) 2014-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <addresstype.h>
#include <clientversion.h>
#include <coins.h>
#include <streams.h>
#include <test/util/poolresourcetester.h>
#include <test/util/random.h>
#include <test/util/setup_common.h>
#include <txdb.h>
#include <uint256.h>
#include <undo.h>
#include <util/strencodings.h>

#include <map>
#include <vector>

#include <boost/test/unit_test.hpp>

int ApplyTxInUndo(Coin&& undo, CCoinsViewCache& view, const COutPoint& out);
void UpdateCoins(const CTransaction& tx, CCoinsViewCache& inputs, CTxUndo &txundo, int nHeight);

namespace
{
//! equality test
bool operator==(const Coin &a, const Coin &b) {
    // Empty Coin objects are always equal.
    if (a.IsSpent() && b.IsSpent()) return true;
    return a.fCoinBase == b.fCoinBase &&
           a.nHeight == b.nHeight &&
           a.out == b.out;
}

class CCoinsViewTest : public CCoinsView
{
    uint256 hashBestBlock_;
    std::map<COutPoint, Coin> map_;

public:
    [[nodiscard]] bool GetCoin(const COutPoint& outpoint, Coin& coin) const override
    {
        std::map<COutPoint, Coin>::const_iterator it = map_.find(outpoint);
        if (it == map_.end()) {
            return false;
        }
        coin = it->second;
        if (coin.IsSpent() && InsecureRandBool() == 0) {
            // Randomly return false in case of an empty entry.
            return false;
        }
        return true;
    }

    uint256 GetBestBlock() const override { return hashBestBlock_; }

    bool BatchWrite(CoinsViewCacheCursor& cursor, const uint256& hashBlock) override
    {
        for (auto it{cursor.Begin()}; it != cursor.End(); it = cursor.NextAndMaybeErase(*it)){
            if (it->second.IsDirty()) {
                // Same optimization used in CCoinsViewDB is to only write dirty entries.
                map_[it->first] = it->second.coin;
                if (it->second.coin.IsSpent() && InsecureRandRange(3) == 0) {
                    // Randomly delete empty entries on write.
                    map_.erase(it->first);
                }
            }
        }
        if (!hashBlock.IsNull())
            hashBestBlock_ = hashBlock;
        return true;
    }
};

class CCoinsViewCacheTest : public CCoinsViewCache
{
public:
    explicit CCoinsViewCacheTest(CCoinsView* _base) : CCoinsViewCache(_base) {}

    void SelfTest(bool sanity_check = true) const
    {
        // Manually recompute the dynamic usage of the whole data, and compare it.
        size_t ret = memusage::DynamicUsage(cacheCoins);
        size_t count = 0;
        for (const auto& entry : cacheCoins) {
            ret += entry.second.coin.DynamicMemoryUsage();
            ++count;
        }
        BOOST_CHECK_EQUAL(GetCacheSize(), count);
        BOOST_CHECK_EQUAL(DynamicMemoryUsage(), ret);
        if (sanity_check) {
            SanityCheck();
        }
    }

    CCoinsMap& map() const { return cacheCoins; }
    CoinsCachePair& sentinel() const { return m_sentinel; }
    size_t& usage() const { return cachedCoinsUsage; }
};

} // namespace

BOOST_FIXTURE_TEST_SUITE(coins_tests, BasicTestingSetup)

static const unsigned int NUM_SIMULATION_ITERATIONS = 40000;

// This is a large randomized insert/remove simulation test on a variable-size
// stack of caches on top of CCoinsViewTest.
//
// It will randomly create/update/delete Coin entries to a tip of caches, with
