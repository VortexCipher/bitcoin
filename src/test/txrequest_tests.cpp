// Copyright (c) 2020-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include <txrequest.h>
#include <uint256.h>

#include <test/util/random.h>
#include <test/util/setup_common.h>

#include <algorithm>
#include <functional>
#include <vector>

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(txrequest_tests, BasicTestingSetup)

namespace {

constexpr std::chrono::microseconds MIN_TIME = std::chrono::microseconds::min();
constexpr std::chrono::microseconds MAX_TIME = std::chrono::microseconds::max();
constexpr std::chrono::microseconds MICROSECOND = std::chrono::microseconds{1};
constexpr std::chrono::microseconds NO_TIME = std::chrono::microseconds{0};

/** An Action is a function to call at a particular (simulated) timestamp. */
using Action = std::pair<std::chrono::microseconds, std::function<void()>>;

/** Object that stores actions from multiple interleaved scenarios, and data shared across them.
 *
 * The Scenario below is used to fill this.
 */
struct Runner
{
    /** The TxRequestTracker being tested. */
    TxRequestTracker txrequest;

    /** List of actions to be executed (in order of increasing timestamp). */
    std::vector<Action> actions;

    /** Which node ids have been assigned already (to prevent reuse). */
    std::set<NodeId> peerset;

    /** Which txhashes have been assigned already (to prevent reuse). */
    std::set<uint256> txhashset;

    /** Which (peer, gtxid) combinations are known to be expired. These need to be accumulated here instead of
     *  checked directly in the GetRequestable return value to avoid introducing a dependency between the various
     *  parallel tests. */
    std::multiset<std::pair<NodeId, GenTxid>> expired;
};

std::chrono::microseconds RandomTime8s() { return std::chrono::microseconds{1 + InsecureRandBits(23)}; }
std::chrono::microseconds RandomTime1y() { return std::chrono::microseconds{1 + InsecureRandBits(45)}; }

/** A proxy for a Runner that helps build a sequence of consecutive test actions on a TxRequestTracker.
 *
 * Each Scenario is a proxy through which actions for the (sequential) execution of various tests are added to a
 * Runner. The actions from multiple scenarios are then run concurrently, resulting in these tests being performed
 * against a TxRequestTracker in parallel. Every test has its own unique txhashes and NodeIds which are not
 * reused in other tests, and thus they should be independent from each other. Running them in parallel however
 * means that we verify the behavior (w.r.t. one test's txhashes and NodeIds) even when the state of the data
 * structure is more complicated due to the presence of other tests.
 */
class Scenario
{
    Runner& m_runner;
    std::chrono::microseconds m_now;
    std::string m_testname;

public:
    Scenario(Runner& runner, std::chrono::microseconds starttime) : m_runner(runner), m_now(starttime) {}

