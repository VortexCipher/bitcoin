// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <coins.h>
#include <consensus/amount.h>
#include <consensus/tx_verify.h>
#include <node/psbt.h>
#include <policy/policy.h>
#include <policy/settings.h>
#include <tinyformat.h>

#include <numeric>

namespace node {
