// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <validationinterface.h>

#include <chain.h>
#include <consensus/validation.h>
#include <kernel/chain.h>
#include <kernel/mempool_entry.h>
#include <logging.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <util/check.h>
#include <util/task_runner.h>

#include <future>
#include <unordered_map>
#include <utility>

std::string RemovalReasonToString(const MemPoolRemovalReason& r) noexcept;

/**
 * ValidationSignalsImpl manages a list of shared_ptr<CValidationInterface> callbacks.
