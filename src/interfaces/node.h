// Copyright (c) 2018-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_INTERFACES_NODE_H
#define BITCOIN_INTERFACES_NODE_H

#include <common/settings.h>
#include <consensus/amount.h>          // For CAmount
#include <net.h>                       // For NodeId
#include <net_types.h>                 // For banmap_t
#include <netaddress.h>                // For Network
#include <netbase.h>                   // For ConnectionDirection
#include <support/allocators/secure.h> // For SecureString
#include <util/translation.h>

#include <functional>
#include <memory>
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <tuple>
#include <vector>

class BanMan;
class CFeeRate;
class CNodeStats;
class Coin;
class RPCTimerInterface;
