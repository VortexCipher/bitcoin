// Copyright (c) 2014 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "db/db_impl.h"
#include "db/filename.h"
#include "db/version_set.h"
#include "db/write_batch_internal.h"
#include "leveldb/db.h"
#include "leveldb/env.h"
#include "leveldb/write_batch.h"
#include "util/logging.h"
#include "util/testharness.h"
#include "util/testutil.h"

namespace leveldb {

class RecoveryTest {
 public:
  RecoveryTest() : env_(Env::Default()), db_(nullptr) {
    dbname_ = test::TmpDir() + "/recovery_test";
    DestroyDB(dbname_, Options());
    Open();
  }

  ~RecoveryTest() {
    Close();
    DestroyDB(dbname_, Options());
  }

  DBImpl* dbfull() const { return reinterpret_cast<DBImpl*>(db_); }
  Env* env() const { return env_; }

