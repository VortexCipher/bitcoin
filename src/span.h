// Copyright (c) 2018-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_SPAN_H
#define BITCOIN_SPAN_H

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <span>
#include <type_traits>

#ifdef DEBUG
#define CONSTEXPR_IF_NOT_DEBUG
#define ASSERT_IF_DEBUG(x) assert((x))
#else
#define CONSTEXPR_IF_NOT_DEBUG constexpr
#define ASSERT_IF_DEBUG(x)
#endif

#if defined(__clang__)
#if __has_attribute(lifetimebound)
#define SPAN_ATTR_LIFETIMEBOUND [[clang::lifetimebound]]
#else
#define SPAN_ATTR_LIFETIMEBOUND
#endif
#else
#define SPAN_ATTR_LIFETIMEBOUND
#endif
