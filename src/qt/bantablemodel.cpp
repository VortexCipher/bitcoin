// Copyright (c) 2011-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/bantablemodel.h>

#include <interfaces/node.h>
#include <net_types.h> // For banmap_t

#include <utility>

#include <QDateTime>
#include <QList>
#include <QLocale>
#include <QModelIndex>
#include <QVariant>

bool BannedNodeLessThan::operator()(const CCombinedBan& left, const CCombinedBan& right) const
