// Copyright (c) 2018-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test/data/blockfilters.json.h>
#include <test/util/setup_common.h>

#include <blockfilter.h>
#include <core_io.h>
#include <primitives/block.h>
#include <serialize.h>
#include <streams.h>
#include <undo.h>
#include <univalue.h>
#include <util/strencodings.h>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(blockfilter_tests)
