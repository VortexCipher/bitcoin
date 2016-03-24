// Copyright (c) 2011-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/guiutil.h>

#include <qt/bitcoinaddressvalidator.h>
#include <qt/bitcoinunits.h>
#include <qt/platformstyle.h>
#include <qt/qvalidatedlineedit.h>
#include <qt/sendcoinsrecipient.h>

#include <addresstype.h>
#include <base58.h>
#include <chainparams.h>
#include <common/args.h>
#include <interfaces/node.h>
#include <key_io.h>
#include <logging.h>
#include <policy/policy.h>
#include <primitives/transaction.h>
#include <protocol.h>
#include <script/script.h>
#include <util/chaintype.h>
#include <util/exception.h>
#include <util/fs.h>
#include <util/fs_helpers.h>
#include <util/time.h>

#ifdef WIN32
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#endif

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QApplication>
#include <QClipboard>
#include <QDateTime>
#include <QDesktopServices>
#include <QDialog>
#include <QDoubleValidator>
#include <QFileDialog>
#include <QFont>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QGuiApplication>
#include <QJsonObject>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLatin1String>
#include <QLineEdit>
#include <QList>
#include <QLocale>
#include <QMenu>
#include <QMouseEvent>
#include <QPluginLoader>
#include <QProgressDialog>
#include <QRegularExpression>
#include <QScreen>
#include <QSettings>
#include <QShortcut>
#include <QSize>
#include <QStandardPaths>
#include <QString>
#include <QTextDocument> // for Qt::mightBeRichText
#include <QThread>
#include <QUrlQuery>
#include <QtGlobal>

#include <cassert>
#include <chrono>
#include <exception>
#include <fstream>
#include <string>
#include <vector>

#if defined(Q_OS_MACOS)

#include <QProcess>

void ForceActivation();
#endif

using namespace std::chrono_literals;

namespace GUIUtil {

QString dateTimeStr(const QDateTime &date)
{
    return QLocale::system().toString(date.date(), QLocale::ShortFormat) + QString(" ") + date.toString("hh:mm");
}

QString dateTimeStr(qint64 nTime)
{
    return dateTimeStr(QDateTime::fromSecsSinceEpoch(nTime));
}

QFont fixedPitchFont(bool use_embedded_font)
{
    if (use_embedded_font) {
