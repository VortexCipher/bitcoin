// Copyright (c) 2011-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/transactiondesc.h>

#include <qt/bitcoinunits.h>
#include <qt/guiutil.h>
#include <qt/paymentserver.h>
#include <qt/transactionrecord.h>

#include <common/system.h>
#include <consensus/consensus.h>
#include <interfaces/node.h>
#include <interfaces/wallet.h>
#include <key_io.h>
#include <logging.h>
#include <policy/policy.h>
#include <validation.h>
#include <wallet/types.h>

#include <stdint.h>
#include <string>

#include <QLatin1String>

using wallet::ISMINE_ALL;
using wallet::ISMINE_SPENDABLE;
using wallet::ISMINE_WATCH_ONLY;
using wallet::isminetype;

QString TransactionDesc::FormatTxStatus(const interfaces::WalletTxStatus& status, bool inMempool)
{
    int depth = status.depth_in_main_chain;
    if (depth < 0) {
        /*: Text explaining the current status of a transaction, shown in the
            status field of the details window for this transaction. This status
            represents an unconfirmed transaction that conflicts with a confirmed
