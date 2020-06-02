// Copyright 2014 BitPay Inc.
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or https://opensource.org/licenses/mit-license.php.

#include <univalue.h>

#include <cassert>
#include <cstdio>
#include <string>

#ifndef JSON_TEST_SRC
#error JSON_TEST_SRC must point to test source directory
#endif

std::string srcdir(JSON_TEST_SRC);

static std::string rtrim(std::string s)
