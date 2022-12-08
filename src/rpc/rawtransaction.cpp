// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <base58.h>
#include <chain.h>
#include <coins.h>
#include <consensus/amount.h>
#include <consensus/validation.h>
#include <core_io.h>
#include <index/txindex.h>
#include <key_io.h>
#include <node/blockstorage.h>
#include <node/coin.h>
#include <node/context.h>
#include <node/psbt.h>
#include <node/transaction.h>
#include <node/types.h>
#include <policy/packages.h>
#include <policy/policy.h>
#include <policy/rbf.h>
#include <primitives/transaction.h>
#include <psbt.h>
#include <random.h>
#include <rpc/blockchain.h>
#include <rpc/rawtransaction_util.h>
#include <rpc/server.h>
#include <rpc/server_util.h>
#include <rpc/util.h>
#include <script/script.h>
#include <script/sign.h>
#include <script/signingprovider.h>
#include <script/solver.h>
#include <uint256.h>
#include <undo.h>
#include <util/bip32.h>
#include <util/check.h>
#include <util/strencodings.h>
#include <util/string.h>
#include <util/vector.h>
#include <validation.h>
#include <validationinterface.h>

#include <numeric>
#include <stdint.h>

#include <univalue.h>

using node::AnalyzePSBT;
using node::FindCoins;
using node::GetTransaction;
using node::NodeContext;
using node::PSBTAnalysis;

static void TxToJSON(const CTransaction& tx, const uint256 hashBlock, UniValue& entry,
                     Chainstate& active_chainstate, const CTxUndo* txundo = nullptr,
                     TxVerbosity verbosity = TxVerbosity::SHOW_DETAILS)
{
    CHECK_NONFATAL(verbosity >= TxVerbosity::SHOW_DETAILS);
    // Call into TxToUniv() in bitcoin-common to decode the transaction hex.
    //
    // Blockchain contextual information (confirmations and blocktime) is not
    // available to code in bitcoin-common, so we query them here and push the
    // data into the returned UniValue.
    TxToUniv(tx, /*block_hash=*/uint256(), entry, /*include_hex=*/true, txundo, verbosity);

    if (!hashBlock.IsNull()) {
        LOCK(cs_main);

        entry.pushKV("blockhash", hashBlock.GetHex());
        const CBlockIndex* pindex = active_chainstate.m_blockman.LookupBlockIndex(hashBlock);
        if (pindex) {
            if (active_chainstate.m_chain.Contains(pindex)) {
                entry.pushKV("confirmations", 1 + active_chainstate.m_chain.Height() - pindex->nHeight);
                entry.pushKV("time", pindex->GetBlockTime());
                entry.pushKV("blocktime", pindex->GetBlockTime());
            }
            else
                entry.pushKV("confirmations", 0);
        }
    }
}

static std::vector<RPCResult> ScriptPubKeyDoc() {
    return
         {
             {RPCResult::Type::STR, "asm", "Disassembly of the output script"},
             {RPCResult::Type::STR, "desc", "Inferred descriptor for the output"},
             {RPCResult::Type::STR_HEX, "hex", "The raw output script bytes, hex-encoded"},
             {RPCResult::Type::STR, "address", /*optional=*/true, "The Bitcoin address (only if a well-defined address exists)"},
             {RPCResult::Type::STR, "type", "The type (one of: " + GetAllOutputTypes() + ")"},
         };
}

static std::vector<RPCResult> DecodeTxDoc(const std::string& txid_field_doc)
{
    return {
        {RPCResult::Type::STR_HEX, "txid", txid_field_doc},
        {RPCResult::Type::STR_HEX, "hash", "The transaction hash (differs from txid for witness transactions)"},
        {RPCResult::Type::NUM, "size", "The serialized transaction size"},
        {RPCResult::Type::NUM, "vsize", "The virtual transaction size (differs from size for witness transactions)"},
        {RPCResult::Type::NUM, "weight", "The transaction's weight (between vsize*4-3 and vsize*4)"},
        {RPCResult::Type::NUM, "version", "The version"},
        {RPCResult::Type::NUM_TIME, "locktime", "The lock time"},
        {RPCResult::Type::ARR, "vin", "",
        {
            {RPCResult::Type::OBJ, "", "",
            {
                {RPCResult::Type::STR_HEX, "coinbase", /*optional=*/true, "The coinbase value (only if coinbase transaction)"},
                {RPCResult::Type::STR_HEX, "txid", /*optional=*/true, "The transaction id (if not coinbase transaction)"},
                {RPCResult::Type::NUM, "vout", /*optional=*/true, "The output number (if not coinbase transaction)"},
                {RPCResult::Type::OBJ, "scriptSig", /*optional=*/true, "The script (if not coinbase transaction)",
                {
                    {RPCResult::Type::STR, "asm", "Disassembly of the signature script"},
                    {RPCResult::Type::STR_HEX, "hex", "The raw signature script bytes, hex-encoded"},
                }},
                {RPCResult::Type::ARR, "txinwitness", /*optional=*/true, "",
                {
                    {RPCResult::Type::STR_HEX, "hex", "hex-encoded witness data (if any)"},
                }},
                {RPCResult::Type::NUM, "sequence", "The script sequence number"},
            }},
        }},
        {RPCResult::Type::ARR, "vout", "",
        {
            {RPCResult::Type::OBJ, "", "",
            {
                {RPCResult::Type::STR_AMOUNT, "value", "The value in " + CURRENCY_UNIT},
                {RPCResult::Type::NUM, "n", "index"},
                {RPCResult::Type::OBJ, "scriptPubKey", "", ScriptPubKeyDoc()},
            }},
        }},
    };
}

static std::vector<RPCArg> CreateTxDoc()
{
    return {
        {"inputs", RPCArg::Type::ARR, RPCArg::Optional::NO, "The inputs",
            {
                {"", RPCArg::Type::OBJ, RPCArg::Optional::OMITTED, "",
                    {
                        {"txid", RPCArg::Type::STR_HEX, RPCArg::Optional::NO, "The transaction id"},
                        {"vout", RPCArg::Type::NUM, RPCArg::Optional::NO, "The output number"},
                        {"sequence", RPCArg::Type::NUM, RPCArg::DefaultHint{"depends on the value of the 'replaceable' and 'locktime' arguments"}, "The sequence number"},
                    },
                },
            },
        },
        {"outputs", RPCArg::Type::ARR, RPCArg::Optional::NO, "The outputs specified as key-value pairs.\n"
                "Each key may only appear once, i.e. there can only be one 'data' output, and no address may be duplicated.\n"
                "At least one output of either type must be specified.\n"
                "For compatibility reasons, a dictionary, which holds the key-value pairs directly, is also\n"
                "                             accepted as second parameter.",
            {
                {"", RPCArg::Type::OBJ_USER_KEYS, RPCArg::Optional::OMITTED, "",
                    {
                        {"address", RPCArg::Type::AMOUNT, RPCArg::Optional::NO, "A key-value pair. The key (string) is the bitcoin address, the value (float or string) is the amount in " + CURRENCY_UNIT},
                    },
                },
                {"", RPCArg::Type::OBJ, RPCArg::Optional::OMITTED, "",
                    {
                        {"data", RPCArg::Type::STR_HEX, RPCArg::Optional::NO, "A key-value pair. The key must be \"data\", the value is hex-encoded data"},
                    },
                },
            },
         RPCArgOptions{.skip_type_check = true}},
        {"locktime", RPCArg::Type::NUM, RPCArg::Default{0}, "Raw locktime. Non-0 value also locktime-activates inputs"},
        {"replaceable", RPCArg::Type::BOOL, RPCArg::Default{true}, "Marks this transaction as BIP125-replaceable.\n"
                "Allows this transaction to be replaced by a transaction with higher fees. If provided, it is an error if explicit sequence numbers are incompatible."},
    };
}

// Update PSBT with information from the mempool, the UTXO set, the txindex, and the provided descriptors.
// Optionally, sign the inputs that we can using information from the descriptors.
PartiallySignedTransaction ProcessPSBT(const std::string& psbt_string, const std::any& context, const HidingSigningProvider& provider, int sighash_type, bool finalize)
{
    // Unserialize the transactions
    PartiallySignedTransaction psbtx;
    std::string error;
    if (!DecodeBase64PSBT(psbtx, psbt_string, error)) {
        throw JSONRPCError(RPC_DESERIALIZATION_ERROR, strprintf("TX decode failed %s", error));
    }

    if (g_txindex) g_txindex->BlockUntilSyncedToCurrentChain();
    const NodeContext& node = EnsureAnyNodeContext(context);

    // If we can't find the corresponding full transaction for all of our inputs,
    // this will be used to find just the utxos for the segwit inputs for which
    // the full transaction isn't found
    std::map<COutPoint, Coin> coins;

    // Fetch previous transactions:
    // First, look in the txindex and the mempool
    for (unsigned int i = 0; i < psbtx.tx->vin.size(); ++i) {
        PSBTInput& psbt_input = psbtx.inputs.at(i);
        const CTxIn& tx_in = psbtx.tx->vin.at(i);

        // The `non_witness_utxo` is the whole previous transaction
        if (psbt_input.non_witness_utxo) continue;

        CTransactionRef tx;
