// Copyright (c) 2017-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_WALLET_COINSELECTION_H
#define BITCOIN_WALLET_COINSELECTION_H

#include <consensus/amount.h>
#include <consensus/consensus.h>
#include <outputtype.h>
#include <policy/feerate.h>
#include <primitives/transaction.h>
#include <random.h>
#include <util/check.h>
#include <util/insert.h>
#include <util/result.h>

#include <optional>


namespace wallet {
//! lower bound for randomly-chosen target change amount
static constexpr CAmount CHANGE_LOWER{50000};
//! upper bound for randomly-chosen target change amount
static constexpr CAmount CHANGE_UPPER{1000000};

/** A UTXO under consideration for use in funding a new transaction. */
struct COutput {
private:
    /** The output's value minus fees required to spend it and bump its unconfirmed ancestors to the target feerate. */
    std::optional<CAmount> effective_value;

    /** The fee required to spend this output at the transaction's target feerate and to bump its unconfirmed ancestors to the target feerate. */
    std::optional<CAmount> fee;

public:
    /** The outpoint identifying this UTXO */
    COutPoint outpoint;

    /** The output itself */
    CTxOut txout;

    /**
     * Depth in block chain.
     * If > 0: the tx is on chain and has this many confirmations.
     * If = 0: the tx is waiting confirmation.
