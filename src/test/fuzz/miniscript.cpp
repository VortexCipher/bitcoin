// Copyright (c) 2021-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <core_io.h>
#include <hash.h>
#include <key.h>
#include <script/miniscript.h>
#include <script/script.h>
#include <script/signingprovider.h>
#include <test/fuzz/FuzzedDataProvider.h>
#include <test/fuzz/fuzz.h>
#include <test/fuzz/util.h>
#include <util/strencodings.h>

namespace {

using Fragment = miniscript::Fragment;
using NodeRef = miniscript::NodeRef<CPubKey>;
using Node = miniscript::Node<CPubKey>;
using Type = miniscript::Type;
using MsCtx = miniscript::MiniscriptContext;
using miniscript::operator"" _mst;

//! Some pre-computed data for more efficient string roundtrips and to simulate challenges.
struct TestData {
    typedef CPubKey Key;

    // Precomputed public keys, and a dummy signature for each of them.
    std::vector<Key> dummy_keys;
    std::map<Key, int> dummy_key_idx_map;
    std::map<CKeyID, Key> dummy_keys_map;
    std::map<Key, std::pair<std::vector<unsigned char>, bool>> dummy_sigs;
    std::map<XOnlyPubKey, std::pair<std::vector<unsigned char>, bool>> schnorr_sigs;

    // Precomputed hashes of each kind.
    std::vector<std::vector<unsigned char>> sha256;
    std::vector<std::vector<unsigned char>> ripemd160;
    std::vector<std::vector<unsigned char>> hash256;
    std::vector<std::vector<unsigned char>> hash160;
    std::map<std::vector<unsigned char>, std::vector<unsigned char>> sha256_preimages;
    std::map<std::vector<unsigned char>, std::vector<unsigned char>> ripemd160_preimages;
    std::map<std::vector<unsigned char>, std::vector<unsigned char>> hash256_preimages;
    std::map<std::vector<unsigned char>, std::vector<unsigned char>> hash160_preimages;

    //! Set the precomputed data.
    void Init() {
        unsigned char keydata[32] = {1};
        // All our signatures sign (and are required to sign) this constant message.
        constexpr uint256 MESSAGE_HASH{"0000000000000000f5cd94e18b6fe77dd7aca9e35c2b0c9cbd86356c80a71065"};
        // We don't pass additional randomness when creating a schnorr signature.
        const auto EMPTY_AUX{uint256::ZERO};

        for (size_t i = 0; i < 256; i++) {
            keydata[31] = i;
            CKey privkey;
            privkey.Set(keydata, keydata + 32, true);
            const Key pubkey = privkey.GetPubKey();

            dummy_keys.push_back(pubkey);
            dummy_key_idx_map.emplace(pubkey, i);
            dummy_keys_map.insert({pubkey.GetID(), pubkey});
            XOnlyPubKey xonly_pubkey{pubkey};
            dummy_key_idx_map.emplace(xonly_pubkey, i);
            uint160 xonly_hash{Hash160(xonly_pubkey)};
            dummy_keys_map.emplace(xonly_hash, pubkey);

            std::vector<unsigned char> sig, schnorr_sig(64);
            privkey.Sign(MESSAGE_HASH, sig);
            sig.push_back(1); // SIGHASH_ALL
            dummy_sigs.insert({pubkey, {sig, i & 1}});
            assert(privkey.SignSchnorr(MESSAGE_HASH, schnorr_sig, nullptr, EMPTY_AUX));
            schnorr_sig.push_back(1); // Maximally-sized signature has sighash byte
            schnorr_sigs.emplace(XOnlyPubKey{pubkey}, std::make_pair(std::move(schnorr_sig), i & 1));

            std::vector<unsigned char> hash;
            hash.resize(32);
            CSHA256().Write(keydata, 32).Finalize(hash.data());
            sha256.push_back(hash);
            if (i & 1) sha256_preimages[hash] = std::vector<unsigned char>(keydata, keydata + 32);
            CHash256().Write(keydata).Finalize(hash);
            hash256.push_back(hash);
            if (i & 1) hash256_preimages[hash] = std::vector<unsigned char>(keydata, keydata + 32);
            hash.resize(20);
            CRIPEMD160().Write(keydata, 32).Finalize(hash.data());
            assert(hash.size() == 20);
            ripemd160.push_back(hash);
            if (i & 1) ripemd160_preimages[hash] = std::vector<unsigned char>(keydata, keydata + 32);
            CHash160().Write(keydata).Finalize(hash);
            hash160.push_back(hash);
            if (i & 1) hash160_preimages[hash] = std::vector<unsigned char>(keydata, keydata + 32);
        }
    }

    //! Get the (Schnorr or ECDSA, depending on context) signature for this pubkey.
    const std::pair<std::vector<unsigned char>, bool>* GetSig(const MsCtx script_ctx, const Key& key) const {
        if (!miniscript::IsTapscript(script_ctx)) {
            const auto it = dummy_sigs.find(key);
            if (it == dummy_sigs.end()) return nullptr;
            return &it->second;
        } else {
            const auto it = schnorr_sigs.find(XOnlyPubKey{key});
            if (it == schnorr_sigs.end()) return nullptr;
            return &it->second;
        }
    }
} TEST_DATA;

/**
 * Context to parse a Miniscript node to and from Script or text representation.
 * Uses an integer (an index in the dummy keys array from the test data) as keys in order
 * to focus on fuzzing the Miniscript nodes' test representation, not the key representation.
 */
struct ParserContext {
    typedef CPubKey Key;

    const MsCtx script_ctx;

    constexpr ParserContext(MsCtx ctx) noexcept : script_ctx(ctx) {}

    bool KeyCompare(const Key& a, const Key& b) const {
        return a < b;
    }

    std::optional<std::string> ToString(const Key& key) const
    {
