/**********************************************************************
 * Copyright (c) 2018,2021 Pieter Wuille, Greg Maxwell, Gleb Naumenko *
 * Distributed under the MIT software license, see the accompanying   *
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.*
 **********************************************************************/

#include <algorithm>
#include <cstdio>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "../include/minisketch.h"
#include "util.h"

namespace {

uint64_t Combination(uint64_t n, uint64_t k) {
    if (n - k < k) k = n - k;
    uint64_t ret = 1;
    for (uint64_t i = 1; i <= k; ++i) {
        ret = (ret * n) / i;
        --n;
    }
    return ret;
}

/** Create a vector with Minisketch objects, one for each implementation. */
std::vector<Minisketch> CreateSketches(uint32_t bits, size_t capacity) {
