// Copyright 2017 The CRC32C Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "./crc32c_arm64.h"

// In a separate source file to allow this accelerated CRC32C function to be
// compiled with the appropriate compiler flags to enable ARM NEON CRC32C
// instructions.

// This implementation is based on https://github.com/google/leveldb/pull/490.

#include <cstddef>
