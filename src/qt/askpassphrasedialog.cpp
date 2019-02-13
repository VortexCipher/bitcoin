// Copyright (c) 2011-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/askpassphrasedialog.h>
#include <qt/forms/ui_askpassphrasedialog.h>

#include <qt/guiconstants.h>
#include <qt/guiutil.h>
#include <qt/walletmodel.h>

#include <support/allocators/secure.h>

#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>

AskPassphraseDialog::AskPassphraseDialog(Mode _mode, QWidget *parent, SecureString* passphrase_out) :
    QDialog(parent, GUIUtil::dialog_flags),
    ui(new Ui::AskPassphraseDialog),
    mode(_mode),
    m_passphrase_out(passphrase_out)
{
    ui->setupUi(this);

    ui->passEdit1->setMinimumSize(ui->passEdit1->sizeHint());
    ui->passEdit2->setMinimumSize(ui->passEdit2->sizeHint());
    ui->passEdit3->setMinimumSize(ui->passEdit3->sizeHint());

    ui->passEdit1->setMaxLength(MAX_PASSPHRASE_SIZE);
