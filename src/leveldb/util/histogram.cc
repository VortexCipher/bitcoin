// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "util/histogram.h"

#include <math.h>
#include <stdio.h>

#include "port/port.h"

namespace leveldb {

const double Histogram::kBucketLimit[kNumBuckets] = {
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    12,
    14,
    16,
