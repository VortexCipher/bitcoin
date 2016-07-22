// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "helpers/memenv/memenv.h"

#include <string>
#include <vector>

#include "db/db_impl.h"
#include "leveldb/db.h"
#include "leveldb/env.h"
#include "util/testharness.h"

namespace leveldb {

class MemEnvTest {
 public:
  MemEnvTest() : env_(NewMemEnv(Env::Default())) {}
  ~MemEnvTest() { delete env_; }

  Env* env_;
};

TEST(MemEnvTest, Basics) {
