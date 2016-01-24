// Copyright (c) 2012-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <streams.h>
#include <test/util/random.h>
#include <test/util/setup_common.h>
#include <util/fs.h>
#include <util/strencodings.h>

#include <boost/test/unit_test.hpp>

using namespace std::string_literals;

BOOST_FIXTURE_TEST_SUITE(streams_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(xor_file)
{
    fs::path xor_path{m_args.GetDataDirBase() / "test_xor.bin"};
    auto raw_file{[&](const auto& mode) { return fsbridge::fopen(xor_path, mode); }};
    const std::vector<uint8_t> test1{1, 2, 3};
    const std::vector<uint8_t> test2{4, 5};
    const std::vector<std::byte> xor_pat{std::byte{0xff}, std::byte{0x00}};
    {
        // Check errors for missing file
        AutoFile xor_file{raw_file("rb"), xor_pat};
        BOOST_CHECK_EXCEPTION(xor_file << std::byte{}, std::ios_base::failure, HasReason{"AutoFile::write: file handle is nullpt"});
        BOOST_CHECK_EXCEPTION(xor_file >> std::byte{}, std::ios_base::failure, HasReason{"AutoFile::read: file handle is nullpt"});
        BOOST_CHECK_EXCEPTION(xor_file.ignore(1), std::ios_base::failure, HasReason{"AutoFile::ignore: file handle is nullpt"});
    }
    {
#ifdef __MINGW64__
        // Temporary workaround for https://github.com/bitcoin/bitcoin/issues/30210
        const char* mode = "wb";
#else
        const char* mode = "wbx";
#endif
        AutoFile xor_file{raw_file(mode), xor_pat};
        xor_file << test1 << test2;
    }
    {
        // Read raw from disk
        AutoFile non_xor_file{raw_file("rb")};
        std::vector<std::byte> raw(7);
        non_xor_file >> Span{raw};
        BOOST_CHECK_EQUAL(HexStr(raw), "fc01fd03fd04fa");
        // Check that no padding exists
        BOOST_CHECK_EXCEPTION(non_xor_file.ignore(1), std::ios_base::failure, HasReason{"AutoFile::ignore: end of file"});
    }
    {
        AutoFile xor_file{raw_file("rb"), xor_pat};
        std::vector<std::byte> read1, read2;
        xor_file >> read1 >> read2;
        BOOST_CHECK_EQUAL(HexStr(read1), HexStr(test1));
        BOOST_CHECK_EQUAL(HexStr(read2), HexStr(test2));
        // Check that eof was reached
        BOOST_CHECK_EXCEPTION(xor_file >> std::byte{}, std::ios_base::failure, HasReason{"AutoFile::read: end of file"});
