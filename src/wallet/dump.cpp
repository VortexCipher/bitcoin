// Copyright (c) 2020-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <wallet/dump.h>

#include <common/args.h>
#include <util/fs.h>
#include <util/translation.h>
#include <wallet/wallet.h>
#include <wallet/walletdb.h>

#include <algorithm>
#include <fstream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace wallet {
static const std::string DUMP_MAGIC = "BITCOIN_CORE_WALLET_DUMP";
uint32_t DUMP_VERSION = 1;

bool DumpWallet(const ArgsManager& args, WalletDatabase& db, bilingual_str& error)
{
    // Get the dumpfile
    std::string dump_filename = args.GetArg("-dumpfile", "");
    if (dump_filename.empty()) {
        error = _("No dump file provided. To use dump, -dumpfile=<filename> must be provided.");
        return false;
