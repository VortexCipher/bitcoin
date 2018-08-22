// Copyright 2008 The CRC32C Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "./crc32c_sse42.h"

// In a separate source file to allow this accelerated CRC32C function to be
// compiled with the appropriate compiler flags to enable SSE4.2 instructions.

// This implementation is loosely based on Intel Pub 323405 from April 2011,
// "Fast CRC Computation for iSCSI Polynomial Using CRC32 Instruction".

#include <cstddef>
#include <cstdint>

#include "./crc32c_internal.h"
#include "./crc32c_prefetch.h"
#include "./crc32c_read_le.h"
#include "./crc32c_round_up.h"
#ifdef CRC32C_HAVE_CONFIG_H
#include "crc32c/crc32c_config.h"
#endif

#if HAVE_SSE42 && (defined(_M_X64) || defined(__x86_64__))

