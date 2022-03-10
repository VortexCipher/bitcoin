// Copyright (c) 2011-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/walletframe.h>

#include <node/interface_ui.h>
#include <psbt.h>
#include <qt/guiutil.h>
#include <qt/overviewpage.h>
#include <qt/psbtoperationsdialog.h>
#include <qt/walletmodel.h>
#include <qt/walletview.h>
#include <util/fs.h>
#include <util/fs_helpers.h>

#include <cassert>
#include <fstream>
#include <string>

#include <QApplication>
#include <QClipboard>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

