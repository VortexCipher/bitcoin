// Copyright (c) 2011-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/peertablemodel.h>

#include <qt/guiconstants.h>
#include <qt/guiutil.h>

#include <interfaces/node.h>

#include <utility>

#include <QList>
#include <QTimer>

PeerTableModel::PeerTableModel(interfaces::Node& node, QObject* parent)
    : QAbstractTableModel(parent),
      m_node(node)
