// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// An Env is an interface used by the leveldb implementation to access
// operating system functionality like the filesystem etc.  Callers
// may wish to provide a custom Env object when opening a database to
// get fine gain control; e.g., to rate limit file system operations.
//
// All Env implementations are safe for concurrent access from
// multiple threads without any external synchronization.

#ifndef STORAGE_LEVELDB_INCLUDE_ENV_H_
#define STORAGE_LEVELDB_INCLUDE_ENV_H_

#include <stdarg.h>
#include <stdint.h>

#include <string>
#include <vector>

#include "leveldb/export.h"
#include "leveldb/status.h"

#if defined(_WIN32)
// The leveldb::Env class below contains a DeleteFile method.
// At the same time, <windows.h>, a fairly popular header
// file for Windows applications, defines a DeleteFile macro.
//
// Without any intervention on our part, the result of this
// unfortunate coincidence is that the name of the
// leveldb::Env::DeleteFile method seen by the compiler depends on
// whether <windows.h> was included before or after the LevelDB
// headers.
//
// To avoid headaches, we undefined DeleteFile (if defined) and
// redefine it at the bottom of this file. This way <windows.h>
// can be included before this file (or not at all) and the
// exported method will always be leveldb::Env::DeleteFile.
