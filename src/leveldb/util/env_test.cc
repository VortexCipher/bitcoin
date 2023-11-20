// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "leveldb/env.h"

#include <algorithm>

#include "port/port.h"
#include "port/thread_annotations.h"
#include "util/mutexlock.h"
#include "util/testharness.h"
#include "util/testutil.h"

namespace leveldb {

static const int kDelayMicros = 100000;

class EnvTest {
 public:
  EnvTest() : env_(Env::Default()) {}

  Env* env_;
