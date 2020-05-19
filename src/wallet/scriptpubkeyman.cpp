// Copyright (c) 2019-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <hash.h>
#include <key_io.h>
#include <logging.h>
#include <node/types.h>
#include <outputtype.h>
#include <script/descriptor.h>
#include <script/script.h>
#include <script/sign.h>
#include <script/solver.h>
#include <util/bip32.h>
#include <util/check.h>
#include <util/strencodings.h>
#include <util/string.h>
#include <util/time.h>
#include <util/translation.h>
#include <wallet/scriptpubkeyman.h>

#include <optional>

using common::PSBTError;
using util::ToString;

namespace wallet {
//! Value for the first BIP 32 hardened derivation. Can be used as a bit mask and as a value. See BIP 32 for more details.
const uint32_t BIP32_HARDENED_KEY_LIMIT = 0x80000000;

util::Result<CTxDestination> LegacyScriptPubKeyMan::GetNewDestination(const OutputType type)
{
    if (LEGACY_OUTPUT_TYPES.count(type) == 0) {
        return util::Error{_("Error: Legacy wallets only support the \"legacy\", \"p2sh-segwit\", and \"bech32\" address types")};
    }
    assert(type != OutputType::BECH32M);

    // Fill-up keypool if needed
    TopUp();

    LOCK(cs_KeyStore);

    // Generate a new key that is added to wallet
    CPubKey new_key;
    if (!GetKeyFromPool(new_key, type)) {
        return util::Error{_("Error: Keypool ran out, please call keypoolrefill first")};
    }
    LearnRelatedScripts(new_key, type);
    return GetDestinationForKey(new_key, type);
}

typedef std::vector<unsigned char> valtype;

namespace {

/**
 * This is an enum that tracks the execution context of a script, similar to
 * SigVersion in script/interpreter. It is separate however because we want to
 * distinguish between top-level scriptPubKey execution and P2SH redeemScript
 * execution (a distinction that has no impact on consensus rules).
 */
enum class IsMineSigVersion
{
    TOP = 0,        //!< scriptPubKey execution
    P2SH = 1,       //!< P2SH redeemScript
    WITNESS_V0 = 2, //!< P2WSH witness script execution
};

/**
 * This is an internal representation of isminetype + invalidity.
 * Its order is significant, as we return the max of all explored
 * possibilities.
 */
enum class IsMineResult
{
    NO = 0,         //!< Not ours
    WATCH_ONLY = 1, //!< Included in watch-only balance
    SPENDABLE = 2,  //!< Included in all balances
    INVALID = 3,    //!< Not spendable by anyone (uncompressed pubkey in segwit, P2SH inside P2SH or witness, witness inside witness)
};

bool PermitsUncompressed(IsMineSigVersion sigversion)
{
    return sigversion == IsMineSigVersion::TOP || sigversion == IsMineSigVersion::P2SH;
}

bool HaveKeys(const std::vector<valtype>& pubkeys, const LegacyDataSPKM& keystore)
{
    for (const valtype& pubkey : pubkeys) {
        CKeyID keyID = CPubKey(pubkey).GetID();
        if (!keystore.HaveKey(keyID)) return false;
    }
    return true;
}

//! Recursively solve script and return spendable/watchonly/invalid status.
//!
//! @param keystore            legacy key and script store
//! @param scriptPubKey        script to solve
//! @param sigversion          script type (top-level / redeemscript / witnessscript)
//! @param recurse_scripthash  whether to recurse into nested p2sh and p2wsh
//!                            scripts or simply treat any script that has been
//!                            stored in the keystore as spendable
// NOLINTNEXTLINE(misc-no-recursion)
IsMineResult IsMineInner(const LegacyDataSPKM& keystore, const CScript& scriptPubKey, IsMineSigVersion sigversion, bool recurse_scripthash=true)
{
    IsMineResult ret = IsMineResult::NO;

    std::vector<valtype> vSolutions;
    TxoutType whichType = Solver(scriptPubKey, vSolutions);

    CKeyID keyID;
    switch (whichType) {
    case TxoutType::NONSTANDARD:
    case TxoutType::NULL_DATA:
    case TxoutType::WITNESS_UNKNOWN:
    case TxoutType::WITNESS_V1_TAPROOT:
    case TxoutType::ANCHOR:
        break;
    case TxoutType::PUBKEY:
        keyID = CPubKey(vSolutions[0]).GetID();
        if (!PermitsUncompressed(sigversion) && vSolutions[0].size() != 33) {
            return IsMineResult::INVALID;
        }
        if (keystore.HaveKey(keyID)) {
            ret = std::max(ret, IsMineResult::SPENDABLE);
        }
        break;
    case TxoutType::WITNESS_V0_KEYHASH:
    {
        if (sigversion == IsMineSigVersion::WITNESS_V0) {
            // P2WPKH inside P2WSH is invalid.
            return IsMineResult::INVALID;
        }
        if (sigversion == IsMineSigVersion::TOP && !keystore.HaveCScript(CScriptID(CScript() << OP_0 << vSolutions[0]))) {
            // We do not support bare witness outputs unless the P2SH version of it would be
            // acceptable as well. This protects against matching before segwit activates.
            // This also applies to the P2WSH case.
            break;
        }
        ret = std::max(ret, IsMineInner(keystore, GetScriptForDestination(PKHash(uint160(vSolutions[0]))), IsMineSigVersion::WITNESS_V0));
        break;
    }
    case TxoutType::PUBKEYHASH:
        keyID = CKeyID(uint160(vSolutions[0]));
        if (!PermitsUncompressed(sigversion)) {
            CPubKey pubkey;
            if (keystore.GetPubKey(keyID, pubkey) && !pubkey.IsCompressed()) {
                return IsMineResult::INVALID;
            }
        }
        if (keystore.HaveKey(keyID)) {
            ret = std::max(ret, IsMineResult::SPENDABLE);
        }
        break;
    case TxoutType::SCRIPTHASH:
    {
        if (sigversion != IsMineSigVersion::TOP) {
            // P2SH inside P2WSH or P2SH is invalid.
            return IsMineResult::INVALID;
        }
        CScriptID scriptID = CScriptID(uint160(vSolutions[0]));
        CScript subscript;
        if (keystore.GetCScript(scriptID, subscript)) {
            ret = std::max(ret, recurse_scripthash ? IsMineInner(keystore, subscript, IsMineSigVersion::P2SH) : IsMineResult::SPENDABLE);
        }
        break;
    }
    case TxoutType::WITNESS_V0_SCRIPTHASH:
    {
        if (sigversion == IsMineSigVersion::WITNESS_V0) {
            // P2WSH inside P2WSH is invalid.
            return IsMineResult::INVALID;
        }
        if (sigversion == IsMineSigVersion::TOP && !keystore.HaveCScript(CScriptID(CScript() << OP_0 << vSolutions[0]))) {
            break;
        }
        CScriptID scriptID{RIPEMD160(vSolutions[0])};
        CScript subscript;
        if (keystore.GetCScript(scriptID, subscript)) {
            ret = std::max(ret, recurse_scripthash ? IsMineInner(keystore, subscript, IsMineSigVersion::WITNESS_V0) : IsMineResult::SPENDABLE);
        }
        break;
    }

    case TxoutType::MULTISIG:
    {
        // Never treat bare multisig outputs as ours (they can still be made watchonly-though)
        if (sigversion == IsMineSigVersion::TOP) {
            break;
        }

        // Only consider transactions "mine" if we own ALL the
        // keys involved. Multi-signature transactions that are
        // partially owned (somebody else has a key that can spend
        // them) enable spend-out-from-under-you attacks, especially
        // in shared-wallet situations.
        std::vector<valtype> keys(vSolutions.begin()+1, vSolutions.begin()+vSolutions.size()-1);
        if (!PermitsUncompressed(sigversion)) {
            for (size_t i = 0; i < keys.size(); i++) {
                if (keys[i].size() != 33) {
                    return IsMineResult::INVALID;
                }
            }
        }
        if (HaveKeys(keys, keystore)) {
            ret = std::max(ret, IsMineResult::SPENDABLE);
        }
        break;
    }
    } // no default case, so the compiler can warn about missing cases

    if (ret == IsMineResult::NO && keystore.HaveWatchOnly(scriptPubKey)) {
        ret = std::max(ret, IsMineResult::WATCH_ONLY);
    }
    return ret;
}

} // namespace

isminetype LegacyDataSPKM::IsMine(const CScript& script) const
{
    switch (IsMineInner(*this, script, IsMineSigVersion::TOP)) {
    case IsMineResult::INVALID:
    case IsMineResult::NO:
        return ISMINE_NO;
    case IsMineResult::WATCH_ONLY:
        return ISMINE_WATCH_ONLY;
    case IsMineResult::SPENDABLE:
        return ISMINE_SPENDABLE;
    }
    assert(false);
}

bool LegacyDataSPKM::CheckDecryptionKey(const CKeyingMaterial& master_key)
{
    {
        LOCK(cs_KeyStore);
        assert(mapKeys.empty());

        bool keyPass = mapCryptedKeys.empty(); // Always pass when there are no encrypted keys
        bool keyFail = false;
        CryptedKeyMap::const_iterator mi = mapCryptedKeys.begin();
        WalletBatch batch(m_storage.GetDatabase());
        for (; mi != mapCryptedKeys.end(); ++mi)
        {
            const CPubKey &vchPubKey = (*mi).second.first;
            const std::vector<unsigned char> &vchCryptedSecret = (*mi).second.second;
            CKey key;
            if (!DecryptKey(master_key, vchCryptedSecret, vchPubKey, key))
            {
                keyFail = true;
                break;
            }
            keyPass = true;
            if (fDecryptionThoroughlyChecked)
                break;
            else {
                // Rewrite these encrypted keys with checksums
                batch.WriteCryptedKey(vchPubKey, vchCryptedSecret, mapKeyMetadata[vchPubKey.GetID()]);
            }
        }
        if (keyPass && keyFail)
        {
            LogPrintf("The wallet is probably corrupted: Some keys decrypt but not all.\n");
            throw std::runtime_error("Error unlocking wallet: some keys decrypt but not all. Your wallet file may be corrupt.");
        }
        if (keyFail || !keyPass)
            return false;
        fDecryptionThoroughlyChecked = true;
    }
    return true;
}

bool LegacyScriptPubKeyMan::Encrypt(const CKeyingMaterial& master_key, WalletBatch* batch)
{
    LOCK(cs_KeyStore);
    encrypted_batch = batch;
    if (!mapCryptedKeys.empty()) {
        encrypted_batch = nullptr;
