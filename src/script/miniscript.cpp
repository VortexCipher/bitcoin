// Copyright (c) 2019-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <string>
#include <vector>
#include <script/script.h>
#include <script/miniscript.h>
#include <serialize.h>

#include <assert.h>

namespace miniscript {
namespace internal {

Type SanitizeType(Type e) {
    int num_types = (e << "K"_mst) + (e << "V"_mst) + (e << "B"_mst) + (e << "W"_mst);
    if (num_types == 0) return ""_mst; // No valid type, don't care about the rest
    assert(num_types == 1); // K, V, B, W all conflict with each other
    assert(!(e << "z"_mst) || !(e << "o"_mst)); // z conflicts with o
    assert(!(e << "n"_mst) || !(e << "z"_mst)); // n conflicts with z
    assert(!(e << "n"_mst) || !(e << "W"_mst)); // n conflicts with W
    assert(!(e << "V"_mst) || !(e << "d"_mst)); // V conflicts with d
    assert(!(e << "K"_mst) ||  (e << "u"_mst)); // K implies u
    assert(!(e << "V"_mst) || !(e << "u"_mst)); // V conflicts with u
    assert(!(e << "e"_mst) || !(e << "f"_mst)); // e conflicts with f
    assert(!(e << "e"_mst) ||  (e << "d"_mst)); // e implies d
    assert(!(e << "V"_mst) || !(e << "e"_mst)); // V conflicts with e
    assert(!(e << "d"_mst) || !(e << "f"_mst)); // d conflicts with f
    assert(!(e << "V"_mst) ||  (e << "f"_mst)); // V implies f
    assert(!(e << "K"_mst) ||  (e << "s"_mst)); // K implies s
    assert(!(e << "z"_mst) ||  (e << "m"_mst)); // z implies m
    return e;
}

Type ComputeType(Fragment fragment, Type x, Type y, Type z, const std::vector<Type>& sub_types, uint32_t k,
                 size_t data_size, size_t n_subs, size_t n_keys, MiniscriptContext ms_ctx) {
    // Sanity check on data
    if (fragment == Fragment::SHA256 || fragment == Fragment::HASH256) {
        assert(data_size == 32);
    } else if (fragment == Fragment::RIPEMD160 || fragment == Fragment::HASH160) {
        assert(data_size == 20);
    } else {
