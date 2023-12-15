// Copyright (c) 2019-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/walletcontroller.h>

#include <qt/askpassphrasedialog.h>
#include <qt/clientmodel.h>
#include <qt/createwalletdialog.h>
#include <qt/guiconstants.h>
#include <qt/guiutil.h>
#include <qt/walletmodel.h>

#include <external_signer.h>
#include <interfaces/handler.h>
#include <interfaces/node.h>
#include <util/string.h>
#include <util/threadnames.h>
#include <util/translation.h>
#include <wallet/wallet.h>

#include <algorithm>
#include <chrono>

#include <QApplication>
#include <QMessageBox>
#include <QMetaObject>
#include <QMutexLocker>
#include <QThread>
#include <QTimer>
#include <QWindow>

using util::Join;
using wallet::WALLET_FLAG_BLANK_WALLET;
using wallet::WALLET_FLAG_DESCRIPTORS;
using wallet::WALLET_FLAG_DISABLE_PRIVATE_KEYS;
using wallet::WALLET_FLAG_EXTERNAL_SIGNER;

WalletController::WalletController(ClientModel& client_model, const PlatformStyle* platform_style, QObject* parent)
    : QObject(parent)
    , m_activity_thread(new QThread(this))
    , m_activity_worker(new QObject)
    , m_client_model(client_model)
    , m_node(client_model.node())
    , m_platform_style(platform_style)
    , m_options_model(client_model.getOptionsModel())
{
    m_handler_load_wallet = m_node.walletLoader().handleLoadWallet([this](std::unique_ptr<interfaces::Wallet> wallet) {
        getOrCreateWallet(std::move(wallet));
