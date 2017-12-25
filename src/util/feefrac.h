// Copyright (c) The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_UTIL_FEEFRAC_H
#define BITCOIN_UTIL_FEEFRAC_H

#include <stdint.h>
#include <compare>
#include <vector>
#include <span.h>
#include <util/check.h>

/** Data structure storing a fee and size, ordered by increasing fee/size.
 *
