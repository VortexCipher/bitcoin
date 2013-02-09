// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_set>
#include <vector>

#include "leveldb/env.h"
#include "port/port.h"
#include "util/env_posix_test_helper.h"
#include "util/testharness.h"

#if HAVE_O_CLOEXEC

namespace {

// Exit codes for the helper process spawned by TestCloseOnExec* tests.
// Useful for debugging test failures.
constexpr int kTextCloseOnExecHelperExecFailedCode = 61;
constexpr int kTextCloseOnExecHelperDup2FailedCode = 62;
constexpr int kTextCloseOnExecHelperFoundOpenFdCode = 63;

// Global set by main() and read in TestCloseOnExec.
//
// The argv[0] value is stored in a std::vector instead of a std::string because
// std::string does not return a mutable pointer to its buffer until C++17.
//
