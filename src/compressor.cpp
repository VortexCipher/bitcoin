// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <compressor.h>

#include <pubkey.h>
#include <script/script.h>

/*
 * These check for scripts for which a special case with a shorter encoding is defined.
 * They are implemented separately from the CScript test, as these test for exact byte
 * sequence correspondences, and are more strict. For example, IsToPubKey also verifies
 * whether the public key is valid (as invalid ones cannot be represented in compressed
 * form).
 */

static bool IsToKeyID(const CScript& script, CKeyID &hash)
