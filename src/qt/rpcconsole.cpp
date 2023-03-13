// Copyright (c) 2011-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <config/bitcoin-config.h> // IWYU pragma: keep

#include <qt/rpcconsole.h>
#include <qt/forms/ui_debugwindow.h>

#include <chainparams.h>
#include <common/system.h>
#include <interfaces/node.h>
#include <node/connection_types.h>
#include <qt/bantablemodel.h>
#include <qt/clientmodel.h>
#include <qt/guiutil.h>
#include <qt/peertablesortproxy.h>
#include <qt/platformstyle.h>
#include <qt/walletmodel.h>
#include <rpc/client.h>
#include <rpc/server.h>
#include <util/strencodings.h>
#include <util/string.h>
#include <util/time.h>
#include <util/threadnames.h>

#include <univalue.h>

#include <QAbstractButton>
#include <QAbstractItemModel>
#include <QDateTime>
#include <QFont>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLatin1String>
#include <QLocale>
#include <QMenu>
#include <QMessageBox>
#include <QScreen>
#include <QScrollBar>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QStyledItemDelegate>
#include <QTime>
#include <QTimer>
#include <QVariant>

#include <chrono>

using util::Join;

const int CONSOLE_HISTORY = 50;
const int INITIAL_TRAFFIC_GRAPH_MINS = 30;
const QSize FONT_RANGE(4, 40);
const char fontSizeSettingsKey[] = "consoleFontSize";

const struct {
    const char *url;
    const char *source;
} ICON_MAPPING[] = {
    {"cmd-request", ":/icons/tx_input"},
    {"cmd-reply", ":/icons/tx_output"},
    {"cmd-error", ":/icons/tx_output"},
    {"misc", ":/icons/tx_inout"},
    {nullptr, nullptr}
};

namespace {

// don't add private key handling cmd's to the history
const QStringList historyFilter = QStringList()
    << "importprivkey"
    << "importmulti"
    << "sethdseed"
    << "signmessagewithprivkey"
    << "signrawtransactionwithkey"
    << "walletpassphrase"
    << "walletpassphrasechange"
    << "encryptwallet";

}

/* Object for executing console RPC commands in a separate thread.
*/
class RPCExecutor : public QObject
{
    Q_OBJECT
public:
    explicit RPCExecutor(interfaces::Node& node) : m_node(node) {}

public Q_SLOTS:
    void request(const QString &command, const WalletModel* wallet_model);

Q_SIGNALS:
    void reply(int category, const QString &command);

private:
    interfaces::Node& m_node;
};

/** Class for handling RPC timers
 * (used for e.g. re-locking the wallet after a timeout)
 */
class QtRPCTimerBase: public QObject, public RPCTimerBase
{
    Q_OBJECT
public:
    QtRPCTimerBase(std::function<void()>& _func, int64_t millis):
        func(_func)
    {
        timer.setSingleShot(true);
        connect(&timer, &QTimer::timeout, [this]{ func(); });
        timer.start(millis);
    }
    ~QtRPCTimerBase() = default;
private:
    QTimer timer;
    std::function<void()> func;
};

class QtRPCTimerInterface: public RPCTimerInterface
{
public:
    ~QtRPCTimerInterface() = default;
    const char *Name() override { return "Qt"; }
    RPCTimerBase* NewTimer(std::function<void()>& func, int64_t millis) override
    {
        return new QtRPCTimerBase(func, millis);
    }
};

class PeerIdViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit PeerIdViewDelegate(QObject* parent = nullptr)
        : QStyledItemDelegate(parent) {}

    QString displayText(const QVariant& value, const QLocale& locale) const override
    {
