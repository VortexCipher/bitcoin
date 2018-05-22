// Copyright (c) 2017-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <key_io.h>
#include <node/types.h>
#include <util/bip32.h>
#include <util/strencodings.h>
#include <wallet/wallet.h>

#include <boost/test/unit_test.hpp>
#include <test/util/setup_common.h>
#include <wallet/test/wallet_test_fixture.h>

namespace wallet {
