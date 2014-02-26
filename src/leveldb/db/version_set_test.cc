// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "db/version_set.h"
#include "util/logging.h"
#include "util/testharness.h"
#include "util/testutil.h"

namespace leveldb {

class FindFileTest {
 public:
  FindFileTest() : disjoint_sorted_files_(true) {}

  ~FindFileTest() {
    for (int i = 0; i < files_.size(); i++) {
      delete files_[i];
    }
  }

  void Add(const char* smallest, const char* largest,
           SequenceNumber smallest_seq = 100,
           SequenceNumber largest_seq = 100) {
    FileMetaData* f = new FileMetaData;
    f->number = files_.size() + 1;
    f->smallest = InternalKey(smallest, smallest_seq, kTypeValue);
    f->largest = InternalKey(largest, largest_seq, kTypeValue);
    files_.push_back(f);
  }

  int Find(const char* key) {
    InternalKey target(key, 100, kTypeValue);
