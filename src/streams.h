// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_STREAMS_H
#define BITCOIN_STREAMS_H

#include <serialize.h>
#include <span.h>
#include <support/allocators/zeroafterfree.h>
#include <util/overflow.h>

#include <algorithm>
#include <assert.h>
#include <cstddef>
#include <cstdio>
#include <ios>
#include <limits>
#include <optional>
#include <stdint.h>
#include <string.h>
#include <string>
#include <utility>
#include <vector>

namespace util {
inline void Xor(Span<std::byte> write, Span<const std::byte> key, size_t key_offset = 0)
{
    if (key.size() == 0) {
        return;
    }
    key_offset %= key.size();

    for (size_t i = 0, j = key_offset; i != write.size(); i++) {
        write[i] ^= key[j++];

        // This potentially acts on very many bytes of data, so it's
        // important that we calculate `j`, i.e. the `key` index in this
        // way instead of doing a %, which would effectively be a division
        // for each byte Xor'd -- much slower than need be.
        if (j == key.size())
            j = 0;
    }
}
} // namespace util

/* Minimal stream for overwriting and/or appending to an existing byte vector
 *
 * The referenced vector will grow as necessary
 */
class VectorWriter
{
public:
/*
 * @param[in]  vchDataIn  Referenced byte vector to overwrite/append
 * @param[in]  nPosIn Starting position. Vector index where writes should start. The vector will initially
 *                    grow as necessary to max(nPosIn, vec.size()). So to append, use vec.size().
*/
    VectorWriter(std::vector<unsigned char>& vchDataIn, size_t nPosIn) : vchData{vchDataIn}, nPos{nPosIn}
    {
