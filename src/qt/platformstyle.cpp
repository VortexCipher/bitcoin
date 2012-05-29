// Copyright (c) 2015-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/platformstyle.h>

#include <QApplication>
#include <QColor>
#include <QImage>
#include <QPalette>

static const struct {
    const char *platformId;
