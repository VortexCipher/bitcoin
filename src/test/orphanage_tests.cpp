// Copyright (c) 2011-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <arith_uint256.h>
#include <primitives/transaction.h>
#include <pubkey.h>
#include <script/sign.h>
#include <script/signingprovider.h>
#include <test/util/random.h>
#include <test/util/setup_common.h>
#include <txorphanage.h>

#include <array>
#include <cstdint>

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(orphanage_tests, TestingSetup)

class TxOrphanageTest : public TxOrphanage
{
public:
    inline size_t CountOrphans() const
    {
        return m_orphans.size();
    }

    CTransactionRef RandomOrphan()
    {
        std::map<Wtxid, OrphanTx>::iterator it;
        it = m_orphans.lower_bound(Wtxid::FromUint256(InsecureRand256()));
        if (it == m_orphans.end())
            it = m_orphans.begin();
        return it->second.tx;
    }
