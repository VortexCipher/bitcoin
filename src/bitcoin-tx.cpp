// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <config/bitcoin-config.h> // IWYU pragma: keep

#include <chainparamsbase.h>
#include <clientversion.h>
#include <coins.h>
#include <common/args.h>
#include <common/system.h>
#include <compat/compat.h>
#include <consensus/amount.h>
#include <consensus/consensus.h>
#include <core_io.h>
#include <key_io.h>
#include <policy/policy.h>
#include <primitives/transaction.h>
#include <script/script.h>
#include <script/sign.h>
#include <script/signingprovider.h>
#include <univalue.h>
#include <util/exception.h>
#include <util/fs.h>
#include <util/moneystr.h>
#include <util/rbf.h>
#include <util/strencodings.h>
#include <util/string.h>
#include <util/translation.h>

#include <cstdio>
#include <functional>
#include <memory>

using util::SplitString;
using util::ToString;
using util::TrimString;
using util::TrimStringView;

static bool fCreateBlank;
static std::map<std::string,UniValue> registers;
static const int CONTINUE_EXECUTION=-1;

const std::function<std::string(const char*)> G_TRANSLATION_FUN = nullptr;

static void SetupBitcoinTxArgs(ArgsManager &argsman)
{
    SetupHelpOptions(argsman);

    argsman.AddArg("-version", "Print version and exit", ArgsManager::ALLOW_ANY, OptionsCategory::OPTIONS);
    argsman.AddArg("-create", "Create new, empty TX.", ArgsManager::ALLOW_ANY, OptionsCategory::OPTIONS);
    argsman.AddArg("-json", "Select JSON output", ArgsManager::ALLOW_ANY, OptionsCategory::OPTIONS);
    argsman.AddArg("-txid", "Output only the hex-encoded transaction id of the resultant transaction.", ArgsManager::ALLOW_ANY, OptionsCategory::OPTIONS);
    SetupChainParamsBaseOptions(argsman);

    argsman.AddArg("delin=N", "Delete input N from TX", ArgsManager::ALLOW_ANY, OptionsCategory::COMMANDS);
    argsman.AddArg("delout=N", "Delete output N from TX", ArgsManager::ALLOW_ANY, OptionsCategory::COMMANDS);
    argsman.AddArg("in=TXID:VOUT(:SEQUENCE_NUMBER)", "Add input to TX", ArgsManager::ALLOW_ANY, OptionsCategory::COMMANDS);
    argsman.AddArg("locktime=N", "Set TX lock time to N", ArgsManager::ALLOW_ANY, OptionsCategory::COMMANDS);
    argsman.AddArg("nversion=N", "Set TX version to N", ArgsManager::ALLOW_ANY, OptionsCategory::COMMANDS);
    argsman.AddArg("outaddr=VALUE:ADDRESS", "Add address-based output to TX", ArgsManager::ALLOW_ANY, OptionsCategory::COMMANDS);
    argsman.AddArg("outdata=[VALUE:]DATA", "Add data-based output to TX", ArgsManager::ALLOW_ANY, OptionsCategory::COMMANDS);
    argsman.AddArg("outmultisig=VALUE:REQUIRED:PUBKEYS:PUBKEY1:PUBKEY2:....[:FLAGS]", "Add Pay To n-of-m Multi-sig output to TX. n = REQUIRED, m = PUBKEYS. "
        "Optionally add the \"W\" flag to produce a pay-to-witness-script-hash output. "
        "Optionally add the \"S\" flag to wrap the output in a pay-to-script-hash.", ArgsManager::ALLOW_ANY, OptionsCategory::COMMANDS);
    argsman.AddArg("outpubkey=VALUE:PUBKEY[:FLAGS]", "Add pay-to-pubkey output to TX. "
        "Optionally add the \"W\" flag to produce a pay-to-witness-pubkey-hash output. "
        "Optionally add the \"S\" flag to wrap the output in a pay-to-script-hash.", ArgsManager::ALLOW_ANY, OptionsCategory::COMMANDS);
    argsman.AddArg("outscript=VALUE:SCRIPT[:FLAGS]", "Add raw script output to TX. "
        "Optionally add the \"W\" flag to produce a pay-to-witness-script-hash output. "
        "Optionally add the \"S\" flag to wrap the output in a pay-to-script-hash.", ArgsManager::ALLOW_ANY, OptionsCategory::COMMANDS);
    argsman.AddArg("replaceable(=N)", "Sets Replace-By-Fee (RBF) opt-in sequence number for input N. "
        "If N is not provided, the command attempts to opt-in all available inputs for RBF. "
        "If the transaction has no inputs, this option is ignored.", ArgsManager::ALLOW_ANY, OptionsCategory::COMMANDS);
    argsman.AddArg("sign=SIGHASH-FLAGS", "Add zero or more signatures to transaction. "
        "This command requires JSON registers:"
        "prevtxs=JSON object, "
        "privatekeys=JSON object. "
        "See signrawtransactionwithkey docs for format of sighash flags, JSON objects.", ArgsManager::ALLOW_ANY, OptionsCategory::COMMANDS);

    argsman.AddArg("load=NAME:FILENAME", "Load JSON file FILENAME into register NAME", ArgsManager::ALLOW_ANY, OptionsCategory::REGISTER_COMMANDS);
    argsman.AddArg("set=NAME:JSON-STRING", "Set register NAME to given JSON-STRING", ArgsManager::ALLOW_ANY, OptionsCategory::REGISTER_COMMANDS);
}

//
// This function returns either one of EXIT_ codes when it's expected to stop the process or
// CONTINUE_EXECUTION when it's expected to continue further.
//
