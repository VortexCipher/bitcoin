// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <config/bitcoin-config.h> // IWYU pragma: keep

#include <chain.h>
#include <clientversion.h>
#include <core_io.h>
#include <hash.h>
#include <interfaces/chain.h>
#include <key_io.h>
#include <merkleblock.h>
#include <rpc/util.h>
#include <script/descriptor.h>
#include <script/script.h>
#include <script/solver.h>
#include <sync.h>
#include <uint256.h>
#include <util/bip32.h>
#include <util/fs.h>
#include <util/time.h>
#include <util/translation.h>
#include <wallet/rpc/util.h>
#include <wallet/wallet.h>

#include <cstdint>
#include <fstream>
#include <tuple>
#include <string>

#include <univalue.h>



using interfaces::FoundBlock;
using util::SplitString;

namespace wallet {
std::string static EncodeDumpString(const std::string &str) {
    std::stringstream ret;
    for (const unsigned char c : str) {
        if (c <= 32 || c >= 128 || c == '%') {
            ret << '%' << HexStr({&c, 1});
        } else {
            ret << c;
        }
    }
    return ret.str();
}

static std::string DecodeDumpString(const std::string &str) {
    std::stringstream ret;
    for (unsigned int pos = 0; pos < str.length(); pos++) {
        unsigned char c = str[pos];
        if (c == '%' && pos+2 < str.length()) {
            c = (((str[pos+1]>>6)*9+((str[pos+1]-'0')&15)) << 4) |
                ((str[pos+2]>>6)*9+((str[pos+2]-'0')&15));
            pos += 2;
        }
        ret << c;
    }
    return ret.str();
}

static bool GetWalletAddressesForKey(const LegacyScriptPubKeyMan* spk_man, const CWallet& wallet, const CKeyID& keyid, std::string& strAddr, std::string& strLabel) EXCLUSIVE_LOCKS_REQUIRED(wallet.cs_wallet)
{
    bool fLabelFound = false;
    CKey key;
    spk_man->GetKey(keyid, key);
    for (const auto& dest : GetAllDestinationsForKey(key.GetPubKey())) {
        const auto* address_book_entry = wallet.FindAddressBookEntry(dest);
        if (address_book_entry) {
            if (!strAddr.empty()) {
                strAddr += ",";
            }
            strAddr += EncodeDestination(dest);
            strLabel = EncodeDumpString(address_book_entry->GetLabel());
            fLabelFound = true;
        }
    }
    if (!fLabelFound) {
        strAddr = EncodeDestination(GetDestinationForKey(key.GetPubKey(), wallet.m_default_address_type));
    }
    return fLabelFound;
}

static const int64_t TIMESTAMP_MIN = 0;

static void RescanWallet(CWallet& wallet, const WalletRescanReserver& reserver, int64_t time_begin = TIMESTAMP_MIN, bool update = true)
{
    int64_t scanned_time = wallet.RescanFromTime(time_begin, reserver, update);
    if (wallet.IsAbortingRescan()) {
        throw JSONRPCError(RPC_MISC_ERROR, "Rescan aborted by user.");
    } else if (scanned_time > time_begin) {
        throw JSONRPCError(RPC_WALLET_ERROR, "Rescan was unable to fully rescan the blockchain. Some transactions may be missing.");
    }
}

static void EnsureBlockDataFromTime(const CWallet& wallet, int64_t timestamp)
{
    auto& chain{wallet.chain()};
    if (!chain.havePruned()) {
        return;
    }

    int height{0};
    const bool found{chain.findFirstBlockWithTimeAndHeight(timestamp - TIMESTAMP_WINDOW, 0, FoundBlock().height(height))};

    uint256 tip_hash{WITH_LOCK(wallet.cs_wallet, return wallet.GetLastBlockHash())};
    if (found && !chain.hasBlocks(tip_hash, height)) {
        throw JSONRPCError(RPC_WALLET_ERROR, strprintf("Pruned blocks from height %d required to import keys. Use RPC call getblockchaininfo to determine your pruned height.", height));
    }
}

RPCHelpMan importprivkey()
{
    return RPCHelpMan{"importprivkey",
                "\nAdds a private key (as returned by dumpprivkey) to your wallet. Requires a new wallet backup.\n"
                "Hint: use importmulti to import more than one private key.\n"
            "\nNote: This call can take over an hour to complete if rescan is true, during that time, other rpc calls\n"
            "may report that the imported key exists but related transactions are still missing, leading to temporarily incorrect/bogus balances and unspent outputs until rescan completes.\n"
            "The rescan parameter can be set to false if the key was never used to create transactions. If it is set to false,\n"
            "but the key was used to create transactions, rescanblockchain needs to be called with the appropriate block range.\n"
            "Note: Use \"getwalletinfo\" to query the scanning progress.\n"
            "Note: This command is only compatible with legacy wallets. Use \"importdescriptors\" with \"combo(X)\" for descriptor wallets.\n",
                {
                    {"privkey", RPCArg::Type::STR, RPCArg::Optional::NO, "The private key (see dumpprivkey)"},
                    {"label", RPCArg::Type::STR, RPCArg::DefaultHint{"current label if address exists, otherwise \"\""}, "An optional label"},
                    {"rescan", RPCArg::Type::BOOL, RPCArg::Default{true}, "Scan the chain and mempool for wallet transactions."},
                },
                RPCResult{RPCResult::Type::NONE, "", ""},
                RPCExamples{
            "\nDump a private key\n"
            + HelpExampleCli("dumpprivkey", "\"myaddress\"") +
            "\nImport the private key with rescan\n"
            + HelpExampleCli("importprivkey", "\"mykey\"") +
            "\nImport using a label and without rescan\n"
            + HelpExampleCli("importprivkey", "\"mykey\" \"testing\" false") +
            "\nImport using default blank label and without rescan\n"
            + HelpExampleCli("importprivkey", "\"mykey\" \"\" false") +
            "\nAs a JSON-RPC call\n"
            + HelpExampleRpc("importprivkey", "\"mykey\", \"testing\", false")
                },
        [&](const RPCHelpMan& self, const JSONRPCRequest& request) -> UniValue
{
    std::shared_ptr<CWallet> const pwallet = GetWalletForJSONRPCRequest(request);
    if (!pwallet) return UniValue::VNULL;

    if (pwallet->IsWalletFlagSet(WALLET_FLAG_DISABLE_PRIVATE_KEYS)) {
        throw JSONRPCError(RPC_WALLET_ERROR, "Cannot import private keys to a wallet with private keys disabled");
    }

    EnsureLegacyScriptPubKeyMan(*pwallet, true);

    WalletRescanReserver reserver(*pwallet);
    bool fRescan = true;
    {
        LOCK(pwallet->cs_wallet);

        EnsureWalletIsUnlocked(*pwallet);

        std::string strSecret = request.params[0].get_str();
        const std::string strLabel{LabelFromValue(request.params[1])};

        // Whether to perform rescan after import
        if (!request.params[2].isNull())
            fRescan = request.params[2].get_bool();

        if (fRescan && pwallet->chain().havePruned()) {
            // Exit early and print an error.
            // If a block is pruned after this check, we will import the key(s),
            // but fail the rescan with a generic error.
            throw JSONRPCError(RPC_WALLET_ERROR, "Rescan is disabled when blocks are pruned");
        }

        if (fRescan && !reserver.reserve()) {
            throw JSONRPCError(RPC_WALLET_ERROR, "Wallet is currently rescanning. Abort existing rescan or wait.");
        }

        CKey key = DecodeSecret(strSecret);
        if (!key.IsValid()) throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid private key encoding");

        CPubKey pubkey = key.GetPubKey();
        CHECK_NONFATAL(key.VerifyPubKey(pubkey));
        CKeyID vchAddress = pubkey.GetID();
        {
            pwallet->MarkDirty();

            // We don't know which corresponding address will be used;
            // label all new addresses, and label existing addresses if a
            // label was passed.
            for (const auto& dest : GetAllDestinationsForKey(pubkey)) {
