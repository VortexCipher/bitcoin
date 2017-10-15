// Copyright (c) 2017 The Zcash developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
//

#include <boost/test/unit_test.hpp>

#include <map>
#include <string>
#include <utility>


std::pair<std::string, std::string> SplitTorReplyLine(const std::string& s);
std::map<std::string, std::string> ParseTorReplyMapping(const std::string& s);


BOOST_AUTO_TEST_SUITE(torcontrol_tests)

static void CheckSplitTorReplyLine(std::string input, std::string command, std::string args)
{
