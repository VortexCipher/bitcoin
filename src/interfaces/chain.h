// Copyright (c) 2018-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_INTERFACES_CHAIN_H
#define BITCOIN_INTERFACES_CHAIN_H

#include <blockfilter.h>
#include <common/settings.h>
#include <primitives/transaction.h> // For CTransactionRef
#include <util/result.h>

#include <functional>
#include <memory>
#include <optional>
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <vector>

class ArgsManager;
class CBlock;
class CBlockUndo;
class CFeeRate;
class CRPCCommand;
class CScheduler;
class Coin;
class uint256;
enum class MemPoolRemovalReason;
enum class RBFTransactionState;
enum class ChainstateRole;
struct bilingual_str;
struct CBlockLocator;
struct FeeCalculation;
namespace node {
struct NodeContext;
} // namespace node

namespace interfaces {

