// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <script/sign.h>

#include <consensus/amount.h>
#include <key.h>
#include <policy/policy.h>
#include <primitives/transaction.h>
#include <script/keyorigin.h>
#include <script/miniscript.h>
#include <script/script.h>
#include <script/signingprovider.h>
#include <script/solver.h>
#include <uint256.h>
#include <util/translation.h>
#include <util/vector.h>

typedef std::vector<unsigned char> valtype;

MutableTransactionSignatureCreator::MutableTransactionSignatureCreator(const CMutableTransaction& tx, unsigned int input_idx, const CAmount& amount, int hash_type)
    : m_txto{tx}, nIn{input_idx}, nHashType{hash_type}, amount{amount}, checker{&m_txto, nIn, amount, MissingDataBehavior::FAIL},
      m_txdata(nullptr)
{
}

MutableTransactionSignatureCreator::MutableTransactionSignatureCreator(const CMutableTransaction& tx, unsigned int input_idx, const CAmount& amount, const PrecomputedTransactionData* txdata, int hash_type)
    : m_txto{tx}, nIn{input_idx}, nHashType{hash_type}, amount{amount},
      checker{txdata ? MutableTransactionSignatureChecker{&m_txto, nIn, amount, *txdata, MissingDataBehavior::FAIL} :
                       MutableTransactionSignatureChecker{&m_txto, nIn, amount, MissingDataBehavior::FAIL}},
      m_txdata(txdata)
{
}

bool MutableTransactionSignatureCreator::CreateSig(const SigningProvider& provider, std::vector<unsigned char>& vchSig, const CKeyID& address, const CScript& scriptCode, SigVersion sigversion) const
{
    assert(sigversion == SigVersion::BASE || sigversion == SigVersion::WITNESS_V0);

    CKey key;
    if (!provider.GetKey(address, key))
        return false;

    // Signing with uncompressed keys is disabled in witness scripts
    if (sigversion == SigVersion::WITNESS_V0 && !key.IsCompressed())
        return false;

    // Signing without known amount does not work in witness scripts.
    if (sigversion == SigVersion::WITNESS_V0 && !MoneyRange(amount)) return false;

    // BASE/WITNESS_V0 signatures don't support explicit SIGHASH_DEFAULT, use SIGHASH_ALL instead.
    const int hashtype = nHashType == SIGHASH_DEFAULT ? SIGHASH_ALL : nHashType;

    uint256 hash = SignatureHash(scriptCode, m_txto, nIn, hashtype, amount, sigversion, m_txdata);
    if (!key.Sign(hash, vchSig))
        return false;
    vchSig.push_back((unsigned char)hashtype);
    return true;
}

bool MutableTransactionSignatureCreator::CreateSchnorrSig(const SigningProvider& provider, std::vector<unsigned char>& sig, const XOnlyPubKey& pubkey, const uint256* leaf_hash, const uint256* merkle_root, SigVersion sigversion) const
{
    assert(sigversion == SigVersion::TAPROOT || sigversion == SigVersion::TAPSCRIPT);

    CKey key;
    if (!provider.GetKeyByXOnly(pubkey, key)) return false;

    // BIP341/BIP342 signing needs lots of precomputed transaction data. While some
    // (non-SIGHASH_DEFAULT) sighash modes exist that can work with just some subset
    // of data present, for now, only support signing when everything is provided.
    if (!m_txdata || !m_txdata->m_bip341_taproot_ready || !m_txdata->m_spent_outputs_ready) return false;

    ScriptExecutionData execdata;
    execdata.m_annex_init = true;
    execdata.m_annex_present = false; // Only support annex-less signing for now.
    if (sigversion == SigVersion::TAPSCRIPT) {
        execdata.m_codeseparator_pos_init = true;
        execdata.m_codeseparator_pos = 0xFFFFFFFF; // Only support non-OP_CODESEPARATOR BIP342 signing for now.
        if (!leaf_hash) return false; // BIP342 signing needs leaf hash.
        execdata.m_tapleaf_hash_init = true;
        execdata.m_tapleaf_hash = *leaf_hash;
    }
    uint256 hash;
    if (!SignatureHashSchnorr(hash, execdata, m_txto, nIn, nHashType, sigversion, *m_txdata, MissingDataBehavior::FAIL)) return false;
