// Copyright (c) 2020-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_I2P_H
#define BITCOIN_I2P_H

#include <compat/compat.h>
#include <netaddress.h>
#include <netbase.h>
#include <sync.h>
#include <util/fs.h>
#include <util/sock.h>
#include <util/threadinterrupt.h>

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace i2p {

/**
 * Binary data.
 */
using Binary = std::vector<uint8_t>;

