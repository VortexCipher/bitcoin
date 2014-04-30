// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_WALLET_WALLETDB_H
#define BITCOIN_WALLET_WALLETDB_H

#include <script/sign.h>
#include <wallet/db.h>
#include <wallet/walletutil.h>
#include <key.h>

#include <stdint.h>
#include <string>
#include <vector>

class CScript;
class uint160;
class uint256;
struct CBlockLocator;

namespace wallet {
class CKeyPool;
class CMasterKey;
class CWallet;
class CWalletTx;
struct WalletContext;

/**
 * Overview of wallet database classes:
 *
