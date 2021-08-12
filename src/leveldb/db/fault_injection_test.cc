// Copyright 2014 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

// This test uses a custom Env to keep track of the state of a filesystem as of
// the last "sync". It then checks for data loss errors by purposely dropping
// file data (or entire files) not protected by a "sync".

#include <map>
#include <set>

#include "db/db_impl.h"
#include "db/filename.h"
#include "db/log_format.h"
#include "db/version_set.h"
#include "leveldb/cache.h"
#include "leveldb/db.h"
#include "leveldb/env.h"
#include "leveldb/table.h"
#include "leveldb/write_batch.h"
#include "port/port.h"
#include "port/thread_annotations.h"
#include "util/logging.h"
#include "util/mutexlock.h"
#include "util/testharness.h"
#include "util/testutil.h"

namespace leveldb {

static const int kValueSize = 1000;
static const int kMaxNumValues = 2000;
static const size_t kNumIterations = 3;

class FaultInjectionTestEnv;

namespace {

// Assume a filename, and not a directory name like "/foo/bar/"
static std::string GetDirName(const std::string& filename) {
  size_t found = filename.find_last_of("/\\");
  if (found == std::string::npos) {
    return "";
  } else {
    return filename.substr(0, found);
  }
}

Status SyncDir(const std::string& dir) {
  // As this is a test it isn't required to *actually* sync this directory.
  return Status::OK();
}

// A basic file truncation function suitable for this test.
Status Truncate(const std::string& filename, uint64_t length) {
  leveldb::Env* env = leveldb::Env::Default();
