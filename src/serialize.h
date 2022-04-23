// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_SERIALIZE_H
#define BITCOIN_SERIALIZE_H

#include <attributes.h>
#include <compat/assumptions.h> // IWYU pragma: keep
#include <compat/endian.h>
#include <prevector.h>
#include <span.h>

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <cstring>
#include <ios>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

/**
 * The maximum size of a serialized object in bytes or number of elements
 * (for eg vectors) when the size is encoded as CompactSize.
 */
static constexpr uint64_t MAX_SIZE = 0x02000000;

/** Maximum amount of memory (in bytes) to allocate at once when deserializing vectors. */
static const unsigned int MAX_VECTOR_ALLOCATE = 5000000;

/**
 * Dummy data type to identify deserializing constructors.
 *
 * By convention, a constructor of a type T with signature
 *
 *   template <typename Stream> T::T(deserialize_type, Stream& s)
 *
 * is a deserializing constructor, which builds the type by
 * deserializing it from s. If T contains const fields, this
 * is likely the only way to do so.
 */
struct deserialize_type {};
constexpr deserialize_type deserialize {};

/*
 * Lowest-level serialization and conversion.
 */
template<typename Stream> inline void ser_writedata8(Stream &s, uint8_t obj)
{
    s.write(AsBytes(Span{&obj, 1}));
}
template<typename Stream> inline void ser_writedata16(Stream &s, uint16_t obj)
{
    obj = htole16_internal(obj);
    s.write(AsBytes(Span{&obj, 1}));
}
template<typename Stream> inline void ser_writedata16be(Stream &s, uint16_t obj)
{
    obj = htobe16_internal(obj);
    s.write(AsBytes(Span{&obj, 1}));
}
template<typename Stream> inline void ser_writedata32(Stream &s, uint32_t obj)
{
    obj = htole32_internal(obj);
    s.write(AsBytes(Span{&obj, 1}));
}
template<typename Stream> inline void ser_writedata32be(Stream &s, uint32_t obj)
{
    obj = htobe32_internal(obj);
    s.write(AsBytes(Span{&obj, 1}));
}
template<typename Stream> inline void ser_writedata64(Stream &s, uint64_t obj)
{
    obj = htole64_internal(obj);
    s.write(AsBytes(Span{&obj, 1}));
}
template<typename Stream> inline uint8_t ser_readdata8(Stream &s)
{
    uint8_t obj;
    s.read(AsWritableBytes(Span{&obj, 1}));
    return obj;
}
template<typename Stream> inline uint16_t ser_readdata16(Stream &s)
{
    uint16_t obj;
    s.read(AsWritableBytes(Span{&obj, 1}));
    return le16toh_internal(obj);
}
template<typename Stream> inline uint16_t ser_readdata16be(Stream &s)
{
    uint16_t obj;
    s.read(AsWritableBytes(Span{&obj, 1}));
    return be16toh_internal(obj);
}
template<typename Stream> inline uint32_t ser_readdata32(Stream &s)
{
    uint32_t obj;
    s.read(AsWritableBytes(Span{&obj, 1}));
    return le32toh_internal(obj);
}
template<typename Stream> inline uint32_t ser_readdata32be(Stream &s)
{
    uint32_t obj;
    s.read(AsWritableBytes(Span{&obj, 1}));
    return be32toh_internal(obj);
}
template<typename Stream> inline uint64_t ser_readdata64(Stream &s)
{
    uint64_t obj;
    s.read(AsWritableBytes(Span{&obj, 1}));
    return le64toh_internal(obj);
}


class SizeComputer;

/**
