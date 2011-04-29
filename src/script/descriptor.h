// Copyright (c) 2018-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_SCRIPT_DESCRIPTOR_H
#define BITCOIN_SCRIPT_DESCRIPTOR_H

#include <outputtype.h>
#include <script/script.h>
#include <script/sign.h>
#include <script/signingprovider.h>

#include <optional>
#include <vector>

using ExtPubKeyMap = std::unordered_map<uint32_t, CExtPubKey>;

/** Cache for single descriptor's derived extended pubkeys */
class DescriptorCache {
private:
