// Copyright (c) 2020-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <crypto/chacha20poly1305.h>
#include <random.h>
#include <span.h>
#include <test/fuzz/FuzzedDataProvider.h>
#include <test/fuzz/fuzz.h>
#include <test/fuzz/util.h>

#include <cstddef>
#include <cstdint>
#include <vector>

constexpr static inline void crypt_till_rekey(FSChaCha20Poly1305& aead, int rekey_interval, bool encrypt)
{
    for (int i = 0; i < rekey_interval; ++i) {
        std::byte dummy_tag[FSChaCha20Poly1305::EXPANSION] = {{}};
        if (encrypt) {
