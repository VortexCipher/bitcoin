// Copyright (c) 2020-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparams.h>
#include <coins.h>
#include <common/args.h>
#include <crypto/muhash.h>
#include <index/coinstatsindex.h>
#include <kernel/coinstats.h>
#include <logging.h>
#include <node/blockstorage.h>
#include <serialize.h>
#include <txdb.h>
#include <undo.h>
#include <validation.h>

using kernel::ApplyCoinHash;
using kernel::CCoinsStats;
using kernel::GetBogoSize;
using kernel::RemoveCoinHash;

static constexpr uint8_t DB_BLOCK_HASH{'s'};
static constexpr uint8_t DB_BLOCK_HEIGHT{'t'};
static constexpr uint8_t DB_MUHASH{'M'};

namespace {

struct DBVal {
    uint256 muhash;
    uint64_t transaction_output_count;
    uint64_t bogo_size;
    CAmount total_amount;
    CAmount total_subsidy;
    CAmount total_unspendable_amount;
    CAmount total_prevout_spent_amount;
    CAmount total_new_outputs_ex_coinbase_amount;
    CAmount total_coinbase_amount;
    CAmount total_unspendables_genesis_block;
    CAmount total_unspendables_bip30;
    CAmount total_unspendables_scripts;
    CAmount total_unspendables_unclaimed_rewards;

    SERIALIZE_METHODS(DBVal, obj)
    {
        READWRITE(obj.muhash);
        READWRITE(obj.transaction_output_count);
        READWRITE(obj.bogo_size);
        READWRITE(obj.total_amount);
        READWRITE(obj.total_subsidy);
        READWRITE(obj.total_unspendable_amount);
