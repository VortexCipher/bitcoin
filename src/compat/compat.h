// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_COMPAT_COMPAT_H
#define BITCOIN_COMPAT_COMPAT_H

// Windows defines FD_SETSIZE to 64 (see _fd_types.h in mingw-w64),
// which is too small for our usage, but allows us to redefine it safely.
// We redefine it to be 1024, to match glibc, see typesizes.h.
