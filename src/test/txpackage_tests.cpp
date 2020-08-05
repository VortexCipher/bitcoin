// Copyright (c) 2021-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <consensus/validation.h>
#include <key_io.h>
#include <policy/packages.h>
#include <policy/policy.h>
#include <policy/rbf.h>
#include <primitives/transaction.h>
#include <script/script.h>
#include <serialize.h>
#include <streams.h>
#include <test/util/random.h>
#include <test/util/script.h>
#include <test/util/setup_common.h>
#include <util/strencodings.h>
#include <test/util/txmempool.h>
#include <validation.h>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(txpackage_tests)
// A fee amount that is above 1sat/vB but below 5sat/vB for most transactions created within these
// unit tests.
static const CAmount low_fee_amt{200};

// Create placeholder transactions that have no meaning.
inline CTransactionRef create_placeholder_tx(size_t num_inputs, size_t num_outputs)
{
    CMutableTransaction mtx = CMutableTransaction();
    mtx.vin.resize(num_inputs);
    mtx.vout.resize(num_outputs);
    auto random_script = CScript() << ToByteVector(InsecureRand256()) << ToByteVector(InsecureRand256());
    for (size_t i{0}; i < num_inputs; ++i) {
        mtx.vin[i].prevout.hash = Txid::FromUint256(InsecureRand256());
        mtx.vin[i].prevout.n = 0;
        mtx.vin[i].scriptSig = random_script;
    }
    for (size_t o{0}; o < num_outputs; ++o) {
        mtx.vout[o].nValue = 1 * CENT;
        mtx.vout[o].scriptPubKey = random_script;
    }
    return MakeTransactionRef(mtx);
}
BOOST_FIXTURE_TEST_CASE(package_hash_tests, TestChain100Setup)
{
    // Random real segwit transaction
    DataStream stream_1{
        ParseHex("02000000000101964b8aa63509579ca6086e6012eeaa4c2f4dd1e283da29b67c8eea38b3c6fd220000000000fdffffff0294c618000000000017a9145afbbb42f4e83312666d0697f9e66259912ecde38768fa2c0000000000160014897388a0889390fd0e153a22bb2cf9d8f019faf50247304402200547406380719f84d68cf4e96cc3e4a1688309ef475b150be2b471c70ea562aa02206d255f5acc40fd95981874d77201d2eb07883657ce1c796513f32b6079545cdf0121023ae77335cefcb5ab4c1dc1fb0d2acfece184e593727d7d5906c78e564c7c11d125cf0c00"),
    };
    CTransaction tx_1(deserialize, TX_WITH_WITNESS, stream_1);
    CTransactionRef ptx_1{MakeTransactionRef(tx_1)};

    // Random real nonsegwit transaction
    DataStream stream_2{
        ParseHex("01000000010b26e9b7735eb6aabdf358bab62f9816a21ba9ebdb719d5299e88607d722c190000000008b4830450220070aca44506c5cef3a16ed519d7c3c39f8aab192c4e1c90d065f37b8a4af6141022100a8e160b856c2d43d27d8fba71e5aef6405b8643ac4cb7cb3c462aced7f14711a0141046d11fee51b0e60666d5049a9101a72741df480b96ee26488a4d3466b95c9a40ac5eeef87e10a5cd336c19a84565f80fa6c547957b7700ff4dfbdefe76036c339ffffffff021bff3d11000000001976a91404943fdd508053c75000106d3bc6e2754dbcff1988ac2f15de00000000001976a914a266436d2965547608b9e15d9032a7b9d64fa43188ac00000000"),
    };
    CTransaction tx_2(deserialize, TX_WITH_WITNESS, stream_2);
    CTransactionRef ptx_2{MakeTransactionRef(tx_2)};

    // Random real segwit transaction
    DataStream stream_3{
        ParseHex("0200000000010177862801f77c2c068a70372b4c435ef8dd621291c36a64eb4dd491f02218f5324600000000fdffffff014a0100000000000022512035ea312034cfac01e956a269f3bf147f569c2fbb00180677421262da042290d803402be713325ff285e66b0380f53f2fae0d0fb4e16f378a440fed51ce835061437566729d4883bc917632f3cff474d6384bc8b989961a1d730d4a87ed38ad28bd337b20f1d658c6c138b1c312e072b4446f50f01ae0da03a42e6274f8788aae53416a7fac0063036f7264010118746578742f706c61696e3b636861727365743d7574662d3800357b2270223a226272632d3230222c226f70223a226d696e74222c227469636b223a224342414c222c22616d74223a2236393639227d6821c1f1d658c6c138b1c312e072b4446f50f01ae0da03a42e6274f8788aae53416a7f00000000"),
    };
    CTransaction tx_3(deserialize, TX_WITH_WITNESS, stream_3);
    CTransactionRef ptx_3{MakeTransactionRef(tx_3)};

    // It's easy to see that wtxids are sorted in lexicographical order:
    Wtxid wtxid_1{Wtxid::FromHex("85cd1a31eb38f74ed5742ec9cb546712ab5aaf747de28a9168b53e846cbda17f").value()};
    Wtxid wtxid_2{Wtxid::FromHex("b4749f017444b051c44dfd2720e88f314ff94f3dd6d56d40ef65854fcd7fff6b").value()};
    Wtxid wtxid_3{Wtxid::FromHex("e065bac15f62bb4e761d761db928ddee65a47296b2b776785abb912cdec474e3").value()};
    BOOST_CHECK_EQUAL(tx_1.GetWitnessHash(), wtxid_1);
    BOOST_CHECK_EQUAL(tx_2.GetWitnessHash(), wtxid_2);
    BOOST_CHECK_EQUAL(tx_3.GetWitnessHash(), wtxid_3);

    BOOST_CHECK(wtxid_1.GetHex() < wtxid_2.GetHex());
    BOOST_CHECK(wtxid_2.GetHex() < wtxid_3.GetHex());

    // The txids are not (we want to test that sorting and hashing use wtxid, not txid):
    Txid txid_1{Txid::FromHex("bd0f71c1d5e50589063e134fad22053cdae5ab2320db5bf5e540198b0b5a4e69").value()};
    Txid txid_2{Txid::FromHex("b4749f017444b051c44dfd2720e88f314ff94f3dd6d56d40ef65854fcd7fff6b").value()};
    Txid txid_3{Txid::FromHex("ee707be5201160e32c4fc715bec227d1aeea5940fb4295605e7373edce3b1a93").value()};
    BOOST_CHECK_EQUAL(tx_1.GetHash(), txid_1);
    BOOST_CHECK_EQUAL(tx_2.GetHash(), txid_2);
    BOOST_CHECK_EQUAL(tx_3.GetHash(), txid_3);

    BOOST_CHECK(txid_2.GetHex() < txid_1.GetHex());

    BOOST_CHECK(txid_1.ToUint256() != wtxid_1.ToUint256());
    BOOST_CHECK(txid_2.ToUint256() == wtxid_2.ToUint256());
    BOOST_CHECK(txid_3.ToUint256() != wtxid_3.ToUint256());

    // We are testing that both functions compare using GetHex() and not uint256.
    // (in this pair of wtxids, hex string order != uint256 order)
    BOOST_CHECK(wtxid_2 < wtxid_1);
    // (in this pair of wtxids, hex string order == uint256 order)
    BOOST_CHECK(wtxid_2 < wtxid_3);

    // All permutations of the package containing ptx_1, ptx_2, ptx_3 have the same package hash
    std::vector<CTransactionRef> package_123{ptx_1, ptx_2, ptx_3};
    std::vector<CTransactionRef> package_132{ptx_1, ptx_3, ptx_2};
    std::vector<CTransactionRef> package_231{ptx_2, ptx_3, ptx_1};
    std::vector<CTransactionRef> package_213{ptx_2, ptx_1, ptx_3};
    std::vector<CTransactionRef> package_312{ptx_3, ptx_1, ptx_2};
    std::vector<CTransactionRef> package_321{ptx_3, ptx_2, ptx_1};

