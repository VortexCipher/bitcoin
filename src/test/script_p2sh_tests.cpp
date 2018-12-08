// Copyright (c) 2012-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <consensus/tx_verify.h>
#include <key.h>
#include <policy/policy.h>
#include <policy/settings.h>
#include <script/script.h>
#include <script/script_error.h>
#include <script/sign.h>
#include <script/signingprovider.h>
#include <test/util/setup_common.h>
#include <validation.h>

#include <vector>

#include <boost/test/unit_test.hpp>

// Helpers:
static bool IsStandardTx(const CTransaction& tx, bool permit_bare_multisig, std::string& reason)
{
    return IsStandardTx(tx, std::nullopt, permit_bare_multisig, CFeeRate{DUST_RELAY_TX_FEE}, reason);
}

static bool IsStandardTx(const CTransaction& tx, std::string& reason)
{
    return IsStandardTx(tx, std::nullopt, /*permit_bare_multisig=*/true, CFeeRate{DUST_RELAY_TX_FEE}, reason) &&
           IsStandardTx(tx, std::nullopt, /*permit_bare_multisig=*/false, CFeeRate{DUST_RELAY_TX_FEE}, reason);
}

static std::vector<unsigned char> Serialize(const CScript& s)
{
    std::vector<unsigned char> sSerialized(s.begin(), s.end());
    return sSerialized;
}

static bool Verify(const CScript& scriptSig, const CScript& scriptPubKey, bool fStrict, ScriptError& err)
{
