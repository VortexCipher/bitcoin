// Copyright (c) 2011-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <config/bitcoin-config.h> // IWYU pragma: keep

#include <qt/notificator.h>

#include <QApplication>
#include <QByteArray>
#include <QImageWriter>
#include <QMessageBox>
#include <QMetaType>
#include <QStyle>
#include <QSystemTrayIcon>
#include <QTemporaryFile>
#include <QVariant>
#ifdef USE_DBUS
#include <QDBusMetaType>
#include <QtDBus>
#include <stdint.h>
#endif
#ifdef Q_OS_MACOS
#include <qt/macnotificationhandler.h>
