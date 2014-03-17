/**********************************************************************
 * Copyright (c) 2018 Pieter Wuille, Greg Maxwell, Gleb Naumenko      *
 * Distributed under the MIT software license, see the accompanying   *
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.*
 **********************************************************************/

#ifndef _MINISKETCH_SKETCH_IMPL_H_
#define _MINISKETCH_SKETCH_IMPL_H_

#include <random>

#include "util.h"
#include "sketch.h"
#include "int_utils.h"

/** Compute the remainder of a polynomial division of val by mod, putting the result in mod. */
template<typename F>
void PolyMod(const std::vector<typename F::Elem>& mod, std::vector<typename F::Elem>& val, const F& field) {
    size_t modsize = mod.size();
    CHECK_SAFE(modsize > 0 && mod.back() == 1);
    if (val.size() < modsize) return;
    CHECK_SAFE(val.back() != 0);
    while (val.size() >= modsize) {
        auto term = val.back();
        val.pop_back();
        if (term != 0) {
            typename F::Multiplier mul(field, term);
            for (size_t x = 0; x < mod.size() - 1; ++x) {
                val[val.size() - modsize + 1 + x] ^= mul(mod[x]);
            }
        }
    }
    while (val.size() > 0 && val.back() == 0) val.pop_back();
}

/** Compute the quotient of a polynomial division of val by mod, putting the quotient in div and the remainder in val. */
template<typename F>
void DivMod(const std::vector<typename F::Elem>& mod, std::vector<typename F::Elem>& val, std::vector<typename F::Elem>& div, const F& field) {
    size_t modsize = mod.size();
    CHECK_SAFE(mod.size() > 0 && mod.back() == 1);
    if (val.size() < mod.size()) {
        div.clear();
        return;
