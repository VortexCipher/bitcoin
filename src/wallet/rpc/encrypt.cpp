// Copyright (c) 2011-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <rpc/util.h>
#include <wallet/rpc/util.h>
#include <wallet/wallet.h>


namespace wallet {
RPCHelpMan walletpassphrase()
{
    return RPCHelpMan{"walletpassphrase",
                "\nStores the wallet decryption key in memory for 'timeout' seconds.\n"
                "This is needed prior to performing transactions related to private keys such as sending bitcoins\n"
            "\nNote:\n"
            "Issuing the walletpassphrase command while the wallet is already unlocked will set a new unlock\n"
            "time that overrides the old one.\n",
                {
                    {"passphrase", RPCArg::Type::STR, RPCArg::Optional::NO, "The wallet passphrase"},
                    {"timeout", RPCArg::Type::NUM, RPCArg::Optional::NO, "The time to keep the decryption key in seconds; capped at 100000000 (~3 years)."},
                },
                RPCResult{RPCResult::Type::NONE, "", ""},
                RPCExamples{
            "\nUnlock the wallet for 60 seconds\n"
            + HelpExampleCli("walletpassphrase", "\"my pass phrase\" 60") +
            "\nLock the wallet again (before 60 seconds)\n"
