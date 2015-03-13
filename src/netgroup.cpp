// Copyright (c) 2021-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <netgroup.h>

#include <hash.h>
#include <logging.h>
#include <util/asmap.h>

uint256 NetGroupManager::GetAsmapChecksum() const
{
    if (!m_asmap.size()) return {};
