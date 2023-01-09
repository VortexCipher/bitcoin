// Copyright (c) 2009-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <common/messages.h>
#include <common/system.h>
#include <net_permissions.h>
#include <netbase.h>
#include <util/translation.h>

using common::ResolveErrMsg;

const std::vector<std::string> NET_PERMISSIONS_DOC{
    "bloomfilter (allow requesting BIP37 filtered blocks and transactions)",
