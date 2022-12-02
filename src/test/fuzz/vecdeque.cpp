// Copyright (c) The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <random.h>
#include <span.h>
#include <test/fuzz/util.h>
#include <util/vecdeque.h>

#include <deque>
#include <stdint.h>

namespace {

/** The maximum number of simultaneous buffers kept by the test. */
static constexpr size_t MAX_BUFFERS{3};
/** How many elements are kept in a buffer at most. */
static constexpr size_t MAX_BUFFER_SIZE{48};
/** How many operations are performed at most on the buffers in one test. */
static constexpr size_t MAX_OPERATIONS{1024};

/** Perform a simulation fuzz test on VecDeque type T.
 *
 * T must be constructible from a uint64_t seed, comparable to other T, copyable, and movable.
 */
template<typename T, bool CheckNoneLeft>
void TestType(Span<const uint8_t> buffer, uint64_t rng_tweak)
{
    FuzzedDataProvider provider(buffer.data(), buffer.size());
    // Local RNG, only used for the seeds to initialize T objects with.
    InsecureRandomContext rng(provider.ConsumeIntegral<uint64_t>() ^ rng_tweak);

    // Real circular buffers.
    std::vector<VecDeque<T>> real;
    real.reserve(MAX_BUFFERS);
    // Simulated circular buffers.
    std::vector<std::deque<T>> sim;
    sim.reserve(MAX_BUFFERS);
    // Temporary object of type T.
    std::optional<T> tmp;

    // Compare a real and a simulated buffer.
    auto compare_fn = [](const VecDeque<T>& r, const std::deque<T>& s) {
        assert(r.size() == s.size());
        assert(r.empty() == s.empty());
        assert(r.capacity() >= r.size());
        if (s.size() == 0) return;
        assert(r.front() == s.front());
        assert(r.back() == s.back());
