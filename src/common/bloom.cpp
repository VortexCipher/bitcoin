// Copyright (c) 2012-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <common/bloom.h>

#include <hash.h>
#include <primitives/transaction.h>
#include <random.h>
#include <script/script.h>
#include <script/solver.h>
#include <span.h>
#include <streams.h>
#include <util/fastrange.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <vector>

static constexpr double LN2SQUARED = 0.4804530139182014246671025263266649717305529515945455;
static constexpr double LN2 = 0.6931471805599453094172321214581765680755001343602552;

