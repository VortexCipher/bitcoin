// Copyright (c) 2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://www.opensource.org/licenses/mit-license.php.

#include <bench/bench.h>
#include <chainparams.h>
#include <wallet/coincontrol.h>
#include <consensus/merkle.h>
#include <kernel/chain.h>
#include <node/context.h>
#include <test/util/setup_common.h>
#include <validation.h>
#include <wallet/spend.h>
#include <wallet/test/util.h>
#include <wallet/wallet.h>

using wallet::CWallet;
using wallet::CreateMockableWalletDatabase;
