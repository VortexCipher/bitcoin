// Copyright (c) 2023-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test/fuzz/util/descriptor.h>

#include <ranges>
#include <stack>

void MockedDescriptorConverter::Init() {
    // The data to use as a private key or a seed for an xprv.
    std::array<std::byte, 32> key_data{std::byte{1}};
    // Generate keys of all kinds and store them in the keys array.
    for (size_t i{0}; i < TOTAL_KEYS_GENERATED; i++) {
