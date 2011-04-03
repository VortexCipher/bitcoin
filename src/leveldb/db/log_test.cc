// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "db/log_reader.h"
#include "db/log_writer.h"
#include "leveldb/env.h"
#include "util/coding.h"
#include "util/crc32c.h"
#include "util/random.h"
#include "util/testharness.h"

namespace leveldb {
namespace log {

// Construct a string of the specified length made out of the supplied
// partial string.
static std::string BigString(const std::string& partial_string, size_t n) {
  std::string result;
  while (result.size() < n) {
    result.append(partial_string);
  }
  result.resize(n);
  return result;
}

// Construct a string from a number
static std::string NumberString(int n) {
  char buf[50];
  snprintf(buf, sizeof(buf), "%d.", n);
  return std::string(buf);
}

// Return a skewed potentially long string
static std::string RandomSkewedString(int i, Random* rnd) {
  return BigString(NumberString(i), rnd->Skewed(17));
}

class LogTest {
 public:
  LogTest()
      : reading_(false),
        writer_(new Writer(&dest_)),
        reader_(new Reader(&source_, &report_, true /*checksum*/,
                           0 /*initial_offset*/)) {}

  ~LogTest() {
    delete writer_;
    delete reader_;
  }

  void ReopenForAppend() {
    delete writer_;
    writer_ = new Writer(&dest_, dest_.contents_.size());
  }

