// Copyright (c) 2021-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <netaddress.h>
#include <net.h>
#include <test/util/net.h>
#include <test/util/setup_common.h>

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <functional>
#include <optional>
#include <unordered_set>
#include <vector>

BOOST_FIXTURE_TEST_SUITE(net_peer_eviction_tests, BasicTestingSetup)

// Create `num_peers` random nodes, apply setup function `candidate_setup_fn`,
// call ProtectEvictionCandidatesByRatio() to apply protection logic, and then
// return true if all of `protected_peer_ids` and none of `unprotected_peer_ids`
// are protected from eviction, i.e. removed from the eviction candidates.
bool IsProtected(int num_peers,
                 std::function<void(NodeEvictionCandidate&)> candidate_setup_fn,
                 const std::unordered_set<NodeId>& protected_peer_ids,
                 const std::unordered_set<NodeId>& unprotected_peer_ids,
                 FastRandomContext& random_context)
{
    std::vector<NodeEvictionCandidate> candidates{GetRandomNodeEvictionCandidates(num_peers, random_context)};
    for (NodeEvictionCandidate& candidate : candidates) {
        candidate_setup_fn(candidate);
    }
    std::shuffle(candidates.begin(), candidates.end(), random_context);

    const size_t size{candidates.size()};
    const size_t expected{size - size / 2}; // Expect half the candidates will be protected.
    ProtectEvictionCandidatesByRatio(candidates);
    BOOST_CHECK_EQUAL(candidates.size(), expected);

    size_t unprotected_count{0};
    for (const NodeEvictionCandidate& candidate : candidates) {
        if (protected_peer_ids.count(candidate.id)) {
            // this peer should have been removed from the eviction candidates
            BOOST_TEST_MESSAGE(strprintf("expected candidate to be protected: %d", candidate.id));
            return false;
        }
        if (unprotected_peer_ids.count(candidate.id)) {
            // this peer remains in the eviction candidates, as expected
            ++unprotected_count;
        }
    }

    const bool is_protected{unprotected_count == unprotected_peer_ids.size()};
    if (!is_protected) {
        BOOST_TEST_MESSAGE(strprintf("unprotected: expected %d, actual %d",
                                     unprotected_peer_ids.size(), unprotected_count));
    }
    return is_protected;
}

BOOST_AUTO_TEST_CASE(peer_protection_test)
{
    FastRandomContext random_context{true};
    int num_peers{12};

    // Expect half of the peers with greatest uptime (the lowest m_connected)
    // to be protected from eviction.
