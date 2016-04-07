// Copyright (c) 2011-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/addressbookpage.h>
#include <qt/forms/ui_addressbookpage.h>

#include <qt/addresstablemodel.h>
#include <qt/csvmodelwriter.h>
#include <qt/editaddressdialog.h>
#include <qt/guiutil.h>
#include <qt/platformstyle.h>

#include <QIcon>
#include <QMenu>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <QRegularExpression>
#else
#include <QRegExp>
#endif

class AddressBookSortFilterProxyModel final : public QSortFilterProxyModel
{
    const QString m_type;

public:
    AddressBookSortFilterProxyModel(const QString& type, QObject* parent)
        : QSortFilterProxyModel(parent)
        , m_type(type)
    {
        setDynamicSortFilter(true);
