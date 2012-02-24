// Copyright (c) 2017-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <key.h>
#include <key_io.h>
#include <node/context.h>
#include <script/script.h>
#include <script/solver.h>
#include <script/signingprovider.h>
#include <test/util/setup_common.h>
#include <wallet/types.h>
#include <wallet/wallet.h>
#include <wallet/test/util.h>

#include <boost/test/unit_test.hpp>


namespace wallet {
BOOST_FIXTURE_TEST_SUITE(ismine_tests, BasicTestingSetup)

wallet::ScriptPubKeyMan* CreateDescriptor(CWallet& keystore, const std::string& desc_str, const bool success)
{
    keystore.SetWalletFlag(WALLET_FLAG_DESCRIPTORS);

    FlatSigningProvider keys;
    std::string error;
    std::unique_ptr<Descriptor> parsed_desc = Parse(desc_str, keys, error, false);
    BOOST_CHECK(success == (parsed_desc != nullptr));
    if (!success) return nullptr;

    const int64_t range_start = 0, range_end = 1, next_index = 0, timestamp = 1;

    WalletDescriptor w_desc(std::move(parsed_desc), timestamp, range_start, range_end, next_index);

    LOCK(keystore.cs_wallet);

    return Assert(keystore.AddWalletDescriptor(w_desc, keys,/*label=*/"", /*internal=*/false));
};

BOOST_AUTO_TEST_CASE(ismine_standard)
{
    CKey keys[2];
    CPubKey pubkeys[2];
    for (int i = 0; i < 2; i++) {
        keys[i].MakeNewKey(true);
        pubkeys[i] = keys[i].GetPubKey();
    }

    CKey uncompressedKey = GenerateRandomKey(/*compressed=*/false);
    CPubKey uncompressedPubkey = uncompressedKey.GetPubKey();
    std::unique_ptr<interfaces::Chain>& chain = m_node.chain;

    CScript scriptPubKey;
    isminetype result;

    // P2PK compressed - Legacy
    {
        CWallet keystore(chain.get(), "", CreateMockableWalletDatabase());
        keystore.SetupLegacyScriptPubKeyMan();
        LOCK(keystore.GetLegacyScriptPubKeyMan()->cs_KeyStore);
        scriptPubKey = GetScriptForRawPubKey(pubkeys[0]);

        // Keystore does not have key
        result = keystore.GetLegacyScriptPubKeyMan()->IsMine(scriptPubKey);
        BOOST_CHECK_EQUAL(result, ISMINE_NO);
        BOOST_CHECK(keystore.GetLegacyScriptPubKeyMan()->GetScriptPubKeys().count(scriptPubKey) == 0);

        // Keystore has key
        BOOST_CHECK(keystore.GetLegacyScriptPubKeyMan()->AddKey(keys[0]));
        result = keystore.GetLegacyScriptPubKeyMan()->IsMine(scriptPubKey);
        BOOST_CHECK_EQUAL(result, ISMINE_SPENDABLE);
