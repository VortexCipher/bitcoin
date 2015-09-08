// Copyright (c) The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <random.h>
#include <span.h>
#include <test/fuzz/util.h>
#include <util/bitset.h>

#include <bitset>
#include <vector>

namespace {

/** Pop the first byte from a byte-span, and return it. */
uint8_t ReadByte(FuzzBufferType& buffer)
{
    if (buffer.empty()) return 0;
    uint8_t ret = buffer.front();
    buffer = buffer.subspan(1);
    return ret;
}

/** Perform a simulation fuzz test on BitSet type S. */
template<typename S>
void TestType(FuzzBufferType buffer)
{
    /** This fuzz test's design is based on the assumption that the actual bits stored in the
     *  bitsets and their simulations do not matter for the purpose of detecting edge cases, thus
     *  these are taken from a deterministically-seeded RNG instead. To provide some level of
     *  variation however, pick the seed based on the buffer size and size of the chosen bitset. */
