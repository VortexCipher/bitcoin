// Copyright (c) 2015-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <prevector.h>
#include <serialize.h>
#include <streams.h>
#include <test/util/random.h>
#include <test/util/setup_common.h>

#include <boost/test/unit_test.hpp>

#include <ranges>
#include <vector>

BOOST_FIXTURE_TEST_SUITE(prevector_tests, TestingSetup)

template<unsigned int N, typename T>
class prevector_tester {
    typedef std::vector<T> realtype;
    realtype real_vector;
    realtype real_vector_alt;

    typedef prevector<N, T> pretype;
    pretype pre_vector;
    pretype pre_vector_alt;

    typedef typename pretype::size_type Size;
    bool passed = true;
