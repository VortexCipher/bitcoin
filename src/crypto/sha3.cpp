// Copyright (c) 2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

// Based on https://github.com/mjosaarinen/tiny_sha3/blob/master/sha3.c
// by Markku-Juhani O. Saarinen <mjos@iki.fi>

#include <crypto/sha3.h>
#include <crypto/common.h>
#include <span.h>

#include <algorithm>
#include <array> // For std::begin and std::end.
#include <bit>

