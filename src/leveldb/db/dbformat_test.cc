// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "db/dbformat.h"
#include "util/logging.h"
#include "util/testharness.h"

namespace leveldb {

static std::string IKey(const std::string& user_key, uint64_t seq,
                        ValueType vt) {
  std::string encoded;
