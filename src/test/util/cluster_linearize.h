// Copyright (c) The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_TEST_UTIL_CLUSTER_LINEARIZE_H
#define BITCOIN_TEST_UTIL_CLUSTER_LINEARIZE_H

#include <cluster_linearize.h>
#include <serialize.h>
#include <span.h>
#include <streams.h>
#include <util/bitset.h>
#include <util/feefrac.h>

#include <stdint.h>
#include <numeric>
#include <vector>
#include <utility>

namespace {

using namespace cluster_linearize;

using TestBitSet = BitSet<32>;

/** Check if a graph is acyclic. */
template<typename SetType>
bool IsAcyclic(const DepGraph<SetType>& depgraph) noexcept
{
    for (ClusterIndex i = 0; i < depgraph.TxCount(); ++i) {
        if ((depgraph.Ancestors(i) & depgraph.Descendants(i)) != SetType::Singleton(i)) {
            return false;
        }
    }
    return true;
