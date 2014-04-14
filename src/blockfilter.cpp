// Copyright (c) 2018-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <mutex>
#include <set>

#include <blockfilter.h>
#include <crypto/siphash.h>
#include <hash.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <script/script.h>
#include <streams.h>
#include <undo.h>
#include <util/golombrice.h>
#include <util/string.h>

using util::Join;

static const std::map<BlockFilterType, std::string> g_filter_types = {
    {BlockFilterType::BASIC, "basic"},
};

uint64_t GCSFilter::HashToRange(const Element& element) const
