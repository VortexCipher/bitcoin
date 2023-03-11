// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_WALLET_WALLET_H
#define BITCOIN_WALLET_WALLET_H

#include <addresstype.h>
#include <consensus/amount.h>
#include <interfaces/chain.h>
#include <interfaces/handler.h>
#include <kernel/cs_main.h>
#include <logging.h>
#include <outputtype.h>
#include <policy/feerate.h>
#include <primitives/transaction.h>
#include <script/interpreter.h>
#include <script/script.h>
#include <support/allocators/secure.h>
#include <sync.h>
#include <tinyformat.h>
#include <uint256.h>
#include <util/fs.h>
#include <util/hasher.h>
#include <util/result.h>
#include <util/string.h>
#include <util/time.h>
#include <util/ui_change_type.h>
#include <wallet/crypter.h>
#include <wallet/db.h>
#include <wallet/scriptpubkeyman.h>
#include <wallet/transaction.h>
#include <wallet/types.h>
#include <wallet/walletutil.h>

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/signals2/signal.hpp>

class CKey;
class CKeyID;
class CPubKey;
class Coin;
class SigningProvider;
enum class MemPoolRemovalReason;
enum class SigningResult;
namespace common {
enum class PSBTError;
} // namespace common
namespace interfaces {
class Wallet;
}
namespace wallet {
class CWallet;
class WalletBatch;
enum class DBErrors : int;
} // namespace wallet
struct CBlockLocator;
struct CExtKey;
struct FlatSigningProvider;
struct KeyOriginInfo;
struct PartiallySignedTransaction;
struct SignatureData;

using LoadWalletFn = std::function<void(std::unique_ptr<interfaces::Wallet> wallet)>;

struct bilingual_str;

namespace wallet {
struct WalletContext;

//! Explicitly unload and delete the wallet.
//! Blocks the current thread after signaling the unload intent so that all
//! wallet pointer owners release the wallet.
//! Note that, when blocking is not required, the wallet is implicitly unloaded
//! by the shared pointer deleter.
void UnloadWallet(std::shared_ptr<CWallet>&& wallet);

bool AddWallet(WalletContext& context, const std::shared_ptr<CWallet>& wallet);
bool RemoveWallet(WalletContext& context, const std::shared_ptr<CWallet>& wallet, std::optional<bool> load_on_start, std::vector<bilingual_str>& warnings);
bool RemoveWallet(WalletContext& context, const std::shared_ptr<CWallet>& wallet, std::optional<bool> load_on_start);
std::vector<std::shared_ptr<CWallet>> GetWallets(WalletContext& context);
std::shared_ptr<CWallet> GetDefaultWallet(WalletContext& context, size_t& count);
std::shared_ptr<CWallet> GetWallet(WalletContext& context, const std::string& name);
std::shared_ptr<CWallet> LoadWallet(WalletContext& context, const std::string& name, std::optional<bool> load_on_start, const DatabaseOptions& options, DatabaseStatus& status, bilingual_str& error, std::vector<bilingual_str>& warnings);
std::shared_ptr<CWallet> CreateWallet(WalletContext& context, const std::string& name, std::optional<bool> load_on_start, DatabaseOptions& options, DatabaseStatus& status, bilingual_str& error, std::vector<bilingual_str>& warnings);
std::shared_ptr<CWallet> RestoreWallet(WalletContext& context, const fs::path& backup_file, const std::string& wallet_name, std::optional<bool> load_on_start, DatabaseStatus& status, bilingual_str& error, std::vector<bilingual_str>& warnings);
std::unique_ptr<interfaces::Handler> HandleLoadWallet(WalletContext& context, LoadWalletFn load_wallet);
void NotifyWalletLoaded(WalletContext& context, const std::shared_ptr<CWallet>& wallet);
std::unique_ptr<WalletDatabase> MakeWalletDatabase(const std::string& name, const DatabaseOptions& options, DatabaseStatus& status, bilingual_str& error);

//! -paytxfee default
constexpr CAmount DEFAULT_PAY_TX_FEE = 0;
//! -fallbackfee default
static const CAmount DEFAULT_FALLBACK_FEE = 0;
//! -discardfee default
static const CAmount DEFAULT_DISCARD_FEE = 10000;
//! -mintxfee default
static const CAmount DEFAULT_TRANSACTION_MINFEE = 1000;
