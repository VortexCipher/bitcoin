// Copyright (c) 2014-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <crypto/sha1.h>

#include <crypto/common.h>

#include <string.h>

// Internal implementation code.
namespace
{
/// Internal SHA-1 implementation.
namespace sha1
{
/** One round of SHA-1. */
void inline Round(uint32_t a, uint32_t& b, uint32_t c, uint32_t d, uint32_t& e, uint32_t f, uint32_t k, uint32_t w)
{
