// Copyright (c) 2018-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <addrdb.h>
#include <banman.h>
#include <blockfilter.h>
#include <chain.h>
#include <chainparams.h>
#include <common/args.h>
#include <consensus/validation.h>
#include <deploymentstatus.h>
#include <external_signer.h>
#include <index/blockfilterindex.h>
#include <init.h>
#include <interfaces/chain.h>
#include <interfaces/handler.h>
#include <interfaces/mining.h>
#include <interfaces/node.h>
#include <interfaces/wallet.h>
#include <kernel/chain.h>
#include <kernel/context.h>
#include <kernel/mempool_entry.h>
#include <logging.h>
#include <mapport.h>
#include <net.h>
#include <net_processing.h>
#include <netaddress.h>
#include <netbase.h>
#include <node/blockstorage.h>
#include <node/coin.h>
#include <node/context.h>
#include <node/interface_ui.h>
#include <node/mini_miner.h>
#include <node/miner.h>
#include <node/transaction.h>
#include <node/types.h>
#include <node/warnings.h>
#include <policy/feerate.h>
#include <policy/fees.h>
#include <policy/policy.h>
#include <policy/rbf.h>
#include <policy/settings.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <rpc/protocol.h>
#include <rpc/server.h>
#include <support/allocators/secure.h>
#include <sync.h>
#include <txmempool.h>
#include <uint256.h>
#include <univalue.h>
#include <util/check.h>
#include <util/result.h>
#include <util/signalinterrupt.h>
#include <util/string.h>
#include <util/translation.h>
#include <validation.h>
#include <validationinterface.h>

#include <config/bitcoin-config.h> // IWYU pragma: keep

#include <any>
#include <memory>
#include <optional>
#include <utility>

#include <boost/signals2/signal.hpp>

using interfaces::BlockTip;
using interfaces::Chain;
using interfaces::FoundBlock;
using interfaces::Handler;
using interfaces::MakeSignalHandler;
using interfaces::Mining;
using interfaces::Node;
using interfaces::WalletLoader;
using node::BlockAssembler;
using util::Join;

namespace node {
// All members of the classes in this namespace are intentionally public, as the
// classes themselves are private.
namespace {
#ifdef ENABLE_EXTERNAL_SIGNER
class ExternalSignerImpl : public interfaces::ExternalSigner
{
public:
    ExternalSignerImpl(::ExternalSigner signer) : m_signer(std::move(signer)) {}
    std::string getName() override { return m_signer.m_name; }
    ::ExternalSigner m_signer;
