// Copyright (c) 2023 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <crypto/chacha20poly1305.h>

#include <crypto/common.h>
#include <crypto/chacha20.h>
#include <crypto/poly1305.h>
#include <span.h>
#include <support/cleanse.h>

#include <assert.h>
