// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <core_io.h>

#include <primitives/block.h>
#include <primitives/transaction.h>
#include <script/script.h>
#include <script/sign.h>
#include <serialize.h>
#include <streams.h>
#include <util/result.h>
#include <util/strencodings.h>

#include <algorithm>
#include <string>

using util::SplitString;

namespace {
class OpCodeParser
{
private:
    std::map<std::string, opcodetype> mapOpNames;
