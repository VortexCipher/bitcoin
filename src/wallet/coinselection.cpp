// Copyright (c) 2017-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <wallet/coinselection.h>

#include <common/system.h>
#include <consensus/amount.h>
#include <consensus/consensus.h>
#include <interfaces/chain.h>
#include <logging.h>
#include <policy/feerate.h>
#include <util/check.h>
#include <util/moneystr.h>

#include <numeric>
#include <optional>
#include <queue>

namespace wallet {
// Common selection error across the algorithms
static util::Result<SelectionResult> ErrorMaxWeightExceeded()
{
    return util::Error{_("The inputs size exceeds the maximum weight. "
                         "Please try sending a smaller amount or manually consolidating your wallet's UTXOs")};
}

// Sort by descending (effective) value prefer lower waste on tie
struct {
    bool operator()(const OutputGroup& a, const OutputGroup& b) const
    {
        if (a.GetSelectionAmount() == b.GetSelectionAmount()) {
            // Lower waste is better when effective_values are tied
            return (a.fee - a.long_term_fee) < (b.fee - b.long_term_fee);
        }
        return a.GetSelectionAmount() > b.GetSelectionAmount();
    }
} descending;

// Sort by descending (effective) value prefer lower weight on tie
struct {
    bool operator()(const OutputGroup& a, const OutputGroup& b) const
    {
        if (a.GetSelectionAmount() == b.GetSelectionAmount()) {
            // Sort lower weight to front on tied effective_value
            return a.m_weight < b.m_weight;
        }
        return a.GetSelectionAmount() > b.GetSelectionAmount();
    }
} descending_effval_weight;

/*
 * This is the Branch and Bound Coin Selection algorithm designed by Murch. It searches for an input
 * set that can pay for the spending target and does not exceed the spending target by more than the
 * cost of creating and spending a change output. The algorithm uses a depth-first search on a binary
 * tree. In the binary tree, each node corresponds to the inclusion or the omission of a UTXO. UTXOs
 * are sorted by their effective values and the tree is explored deterministically per the inclusion
 * branch first. At each node, the algorithm checks whether the selection is within the target range.
 * While the selection has not reached the target range, more UTXOs are included. When a selection's
 * value exceeds the target range, the complete subtree deriving from this selection can be omitted.
 * At that point, the last included UTXO is deselected and the corresponding omission branch explored
 * instead. The search ends after the complete tree has been searched or after a limited number of tries.
 *
 * The search continues to search for better solutions after one solution has been found. The best
 * solution is chosen by minimizing the waste metric. The waste metric is defined as the cost to
 * spend the current inputs at the given fee rate minus the long term expected cost to spend the
 * inputs, plus the amount by which the selection exceeds the spending target:
 *
 * waste = selectionTotal - target + inputs × (currentFeeRate - longTermFeeRate)
 *
 * The algorithm uses two additional optimizations. A lookahead keeps track of the total value of
 * the unexplored UTXOs. A subtree is not explored if the lookahead indicates that the target range
 * cannot be reached. Further, it is unnecessary to test equivalent combinations. This allows us
 * to skip testing the inclusion of UTXOs that match the effective value and waste of an omitted
 * predecessor.
 *
 * The Branch and Bound algorithm is described in detail in Murch's Master Thesis:
 * https://murch.one/wp-content/uploads/2016/11/erhardt2016coinselection.pdf
 *
 * @param const std::vector<OutputGroup>& utxo_pool The set of UTXO groups that we are choosing from.
 *        These UTXO groups will be sorted in descending order by effective value and the OutputGroups'
 *        values are their effective values.
 * @param const CAmount& selection_target This is the value that we want to select. It is the lower
 *        bound of the range.
 * @param const CAmount& cost_of_change This is the cost of creating and spending a change output.
 *        This plus selection_target is the upper bound of the range.
 * @param int max_selection_weight The maximum allowed weight for a selection result to be valid.
 * @returns The result of this coin selection algorithm, or std::nullopt
 */

static const size_t TOTAL_TRIES = 100000;

util::Result<SelectionResult> SelectCoinsBnB(std::vector<OutputGroup>& utxo_pool, const CAmount& selection_target, const CAmount& cost_of_change,
                                             int max_selection_weight)
{
    SelectionResult result(selection_target, SelectionAlgorithm::BNB);
    CAmount curr_value = 0;
    std::vector<size_t> curr_selection; // selected utxo indexes
    int curr_selection_weight = 0; // sum of selected utxo weight
