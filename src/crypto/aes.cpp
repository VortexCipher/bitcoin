// Copyright (c) 2016-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <crypto/aes.h>

#include <string.h>

extern "C" {
#include <crypto/ctaes/ctaes.c>
}

AES256Encrypt::AES256Encrypt(const unsigned char key[32])
{
    AES256_init(&ctx, key);
