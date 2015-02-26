// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <rpc/blockchain.h>

#include <node/mempool_persist.h>

#include <chainparams.h>
#include <core_io.h>
#include <kernel/mempool_entry.h>
#include <node/mempool_persist_args.h>
#include <node/types.h>
#include <policy/rbf.h>
#include <policy/settings.h>
#include <primitives/transaction.h>
#include <rpc/server.h>
#include <rpc/server_util.h>
#include <rpc/util.h>
#include <txmempool.h>
#include <univalue.h>
#include <util/fs.h>
#include <util/moneystr.h>
#include <util/strencodings.h>
#include <util/time.h>

#include <utility>

using node::DumpMempool;

using node::DEFAULT_MAX_BURN_AMOUNT;
using node::DEFAULT_MAX_RAW_TX_FEE_RATE;
using node::MempoolPath;
using node::NodeContext;
using node::TransactionError;
using util::ToString;

static RPCHelpMan sendrawtransaction()
{
    return RPCHelpMan{"sendrawtransaction",
        "\nSubmit a raw transaction (serialized, hex-encoded) to local node and network.\n"
        "\nThe transaction will be sent unconditionally to all peers, so using sendrawtransaction\n"
        "for manual rebroadcast may degrade privacy by leaking the transaction's origin, as\n"
        "nodes will normally not rebroadcast non-wallet transactions already in their mempool.\n"
        "\nA specific exception, RPC_TRANSACTION_ALREADY_IN_UTXO_SET, may throw if the transaction cannot be added to the mempool.\n"
        "\nRelated RPCs: createrawtransaction, signrawtransactionwithkey\n",
        {
            {"hexstring", RPCArg::Type::STR_HEX, RPCArg::Optional::NO, "The hex string of the raw transaction"},
            {"maxfeerate", RPCArg::Type::AMOUNT, RPCArg::Default{FormatMoney(DEFAULT_MAX_RAW_TX_FEE_RATE.GetFeePerK())},
             "Reject transactions whose fee rate is higher than the specified value, expressed in " + CURRENCY_UNIT +
                 "/kvB.\nFee rates larger than 1BTC/kvB are rejected.\nSet to 0 to accept any fee rate."},
            {"maxburnamount", RPCArg::Type::AMOUNT, RPCArg::Default{FormatMoney(DEFAULT_MAX_BURN_AMOUNT)},
             "Reject transactions with provably unspendable outputs (e.g. 'datacarrier' outputs that use the OP_RETURN opcode) greater than the specified value, expressed in " + CURRENCY_UNIT + ".\n"
             "If burning funds through unspendable outputs is desired, increase this value.\n"
             "This check is based on heuristics and does not guarantee spendability of outputs.\n"},
        },
        RPCResult{
            RPCResult::Type::STR_HEX, "", "The transaction hash in hex"
        },
        RPCExamples{
            "\nCreate a transaction\n"
            + HelpExampleCli("createrawtransaction", "\"[{\\\"txid\\\" : \\\"mytxid\\\",\\\"vout\\\":0}]\" \"{\\\"myaddress\\\":0.01}\"") +
            "Sign the transaction, and get back the hex\n"
            + HelpExampleCli("signrawtransactionwithwallet", "\"myhex\"") +
            "\nSend the transaction (signed hex)\n"
            + HelpExampleCli("sendrawtransaction", "\"signedhex\"") +
            "\nAs a JSON-RPC call\n"
            + HelpExampleRpc("sendrawtransaction", "\"signedhex\"")
                },
        [&](const RPCHelpMan& self, const JSONRPCRequest& request) -> UniValue
        {
            const CAmount max_burn_amount = request.params[2].isNull() ? 0 : AmountFromValue(request.params[2]);

            CMutableTransaction mtx;
            if (!DecodeHexTx(mtx, request.params[0].get_str())) {
                throw JSONRPCError(RPC_DESERIALIZATION_ERROR, "TX decode failed. Make sure the tx has at least one input.");
            }

            for (const auto& out : mtx.vout) {
                if((out.scriptPubKey.IsUnspendable() || !out.scriptPubKey.HasValidOps()) && out.nValue > max_burn_amount) {
                    throw JSONRPCTransactionError(TransactionError::MAX_BURN_EXCEEDED);
                }
            }

            CTransactionRef tx(MakeTransactionRef(std::move(mtx)));

            const CFeeRate max_raw_tx_fee_rate{ParseFeeRate(self.Arg<UniValue>("maxfeerate"))};

            int64_t virtual_size = GetVirtualTransactionSize(*tx);
            CAmount max_raw_tx_fee = max_raw_tx_fee_rate.GetFee(virtual_size);

            std::string err_string;
            AssertLockNotHeld(cs_main);
            NodeContext& node = EnsureAnyNodeContext(request.context);
            const TransactionError err = BroadcastTransaction(node, tx, err_string, max_raw_tx_fee, /*relay=*/true, /*wait_callback=*/true);
            if (TransactionError::OK != err) {
                throw JSONRPCTransactionError(err, err_string);
            }

            return tx->GetHash().GetHex();
        },
    };
