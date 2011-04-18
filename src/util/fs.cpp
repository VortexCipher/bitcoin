// Copyright (c) 2017-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <util/fs.h>
#include <util/syserror.h>

#ifndef WIN32
#include <cstring>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/utsname.h>
#include <unistd.h>
