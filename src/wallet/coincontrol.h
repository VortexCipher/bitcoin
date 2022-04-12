// Copyright (c) 2011-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_WALLET_COINCONTROL_H
#define BITCOIN_WALLET_COINCONTROL_H

#include <outputtype.h>
#include <policy/feerate.h>
#include <policy/fees.h>
#include <primitives/transaction.h>
#include <script/keyorigin.h>
#include <script/signingprovider.h>

#include <algorithm>
#include <map>
#include <optional>
#include <set>

