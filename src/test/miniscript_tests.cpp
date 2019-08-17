// Copyright (c) 2019-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <stdint.h>
#include <string>
#include <vector>

#include <test/util/random.h>
#include <test/util/setup_common.h>
#include <boost/test/unit_test.hpp>

#include <addresstype.h>
#include <core_io.h>
#include <hash.h>
#include <pubkey.h>
#include <uint256.h>
#include <crypto/ripemd160.h>
#include <crypto/sha256.h>
#include <script/interpreter.h>
#include <script/miniscript.h>
#include <script/script_error.h>
#include <script/signingprovider.h>

namespace {

/** TestData groups various kinds of precomputed data necessary in this test. */
struct TestData {
    //! The only public keys used in this test.
    std::vector<CPubKey> pubkeys;
    //! A map from the public keys to their CKeyIDs (faster than hashing every time).
    std::map<CPubKey, CKeyID> pkhashes;
    std::map<CKeyID, CPubKey> pkmap;
    std::map<XOnlyPubKey, CKeyID> xonly_pkhashes;
    std::map<CPubKey, std::vector<unsigned char>> signatures;
    std::map<XOnlyPubKey, std::vector<unsigned char>> schnorr_signatures;

    // Various precomputed hashes
    std::vector<std::vector<unsigned char>> sha256;
    std::vector<std::vector<unsigned char>> ripemd160;
    std::vector<std::vector<unsigned char>> hash256;
    std::vector<std::vector<unsigned char>> hash160;
    std::map<std::vector<unsigned char>, std::vector<unsigned char>> sha256_preimages;
    std::map<std::vector<unsigned char>, std::vector<unsigned char>> ripemd160_preimages;
    std::map<std::vector<unsigned char>, std::vector<unsigned char>> hash256_preimages;
    std::map<std::vector<unsigned char>, std::vector<unsigned char>> hash160_preimages;

    TestData()
    {
        // All our signatures sign (and are required to sign) this constant message.
        constexpr uint256 MESSAGE_HASH{"0000000000000000f5cd94e18b6fe77dd7aca9e35c2b0c9cbd86356c80a71065"};
        // We don't pass additional randomness when creating a schnorr signature.
        const auto EMPTY_AUX{uint256::ZERO};

        // We generate 255 public keys and 255 hashes of each type.
        for (int i = 1; i <= 255; ++i) {
            // This 32-byte array functions as both private key data and hash preimage (31 zero bytes plus any nonzero byte).
            unsigned char keydata[32] = {0};
            keydata[31] = i;

            // Compute CPubkey and CKeyID
            CKey key;
            key.Set(keydata, keydata + 32, true);
            CPubKey pubkey = key.GetPubKey();
            CKeyID keyid = pubkey.GetID();
            pubkeys.push_back(pubkey);
            pkhashes.emplace(pubkey, keyid);
            pkmap.emplace(keyid, pubkey);
            XOnlyPubKey xonly_pubkey{pubkey};
            uint160 xonly_hash{Hash160(xonly_pubkey)};
            xonly_pkhashes.emplace(xonly_pubkey, xonly_hash);
            pkmap.emplace(xonly_hash, pubkey);
