// Copyright (c) The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bench/bench.h>

#include <util/bitset.h>
#include <cluster_linearize.h>

using namespace cluster_linearize;

namespace {

/** Construct a linear graph. These are pessimal for AncestorCandidateFinder, as they maximize
 *  the number of ancestor set feerate updates. The best ancestor set is always the topmost
 *  remaining transaction, whose removal requires updating all remaining transactions' ancestor
 *  set feerates. */
template<typename SetType>
DepGraph<SetType> MakeLinearGraph(ClusterIndex ntx)
{
    DepGraph<SetType> depgraph;
    for (ClusterIndex i = 0; i < ntx; ++i) {
        depgraph.AddTransaction({-int32_t(i), 1});
        if (i > 0) depgraph.AddDependency(i - 1, i);
    }
    return depgraph;
}
