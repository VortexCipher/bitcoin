// Copyright (c) 2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <random.h>
#include <test/fuzz/FuzzedDataProvider.h>
#include <test/fuzz/util.h>
#include <util/bitdeque.h>

#include <deque>
#include <vector>

namespace {

constexpr int LEN_BITS = 16;
constexpr int RANDDATA_BITS = 20;

using bitdeque_type = bitdeque<128>;

//! Deterministic random vector of bools, for begin/end insertions to draw from.
std::vector<bool> RANDDATA;

void InitRandData()
{
    FastRandomContext ctx(true);
    RANDDATA.clear();
    for (size_t i = 0; i < (1U << RANDDATA_BITS) + (1U << LEN_BITS); ++i) {
        RANDDATA.push_back(ctx.randbool());
    }
}

} // namespace

FUZZ_TARGET(bitdeque, .init = InitRandData)
{
    FuzzedDataProvider provider(buffer.data(), buffer.size());
    FastRandomContext ctx(true);

    size_t maxlen = (1U << provider.ConsumeIntegralInRange<size_t>(0, LEN_BITS)) - 1;
    size_t limitlen = 4 * maxlen;

    std::deque<bool> deq;
    bitdeque_type bitdeq;

    const auto& cdeq = deq;
    const auto& cbitdeq = bitdeq;

    size_t initlen = provider.ConsumeIntegralInRange<size_t>(0, maxlen);
    while (initlen) {
        bool val = ctx.randbool();
        deq.push_back(val);
        bitdeq.push_back(val);
        --initlen;
    }
