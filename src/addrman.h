// Copyright (c) 2012 Pieter Wuille
// Copyright (c) 2012-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_ADDRMAN_H
#define BITCOIN_ADDRMAN_H

#include <netaddress.h>
#include <netgroup.h>
#include <protocol.h>
#include <streams.h>
#include <util/time.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

class InvalidAddrManVersionError : public std::ios_base::failure
