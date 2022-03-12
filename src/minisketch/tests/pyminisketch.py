#!/usr/bin/env python3
# Copyright (c) 2020 Pieter Wuille
# Distributed under the MIT software license, see the accompanying
# file LICENSE or http://www.opensource.org/licenses/mit-license.php.

"""Native Python (slow) reimplementation of libminisketch' algorithms."""

import random
import unittest

# Irreducible polynomials over GF(2) to use (represented as integers).
#
# Most fields can be defined by multiple such polynomials. Minisketch uses the one with the minimal
# number of nonzero coefficients, and tie-breaking by picking the lexicographically first among
# those.
#
# All polynomials for degrees 2 through 64 (inclusive) are given.
GF2_MODULI = [
    None, None,
    2**2 + 2**1 + 1,
    2**3 + 2**1 + 1,
    2**4 + 2**1 + 1,
    2**5 + 2**2 + 1,
    2**6 + 2**1 + 1,
    2**7 + 2**1 + 1,
    2**8 + 2**4 + 2**3 + 2**1 + 1,
    2**9 + 2**1 + 1,
    2**10 + 2**3 + 1,
    2**11 + 2**2 + 1,
    2**12 + 2**3 + 1,
    2**13 + 2**4 + 2**3 + 2**1 + 1,
    2**14 + 2**5 + 1,
    2**15 + 2**1 + 1,
    2**16 + 2**5 + 2**3 + 2**1 + 1,
    2**17 + 2**3 + 1,
    2**18 + 2**3 + 1,
    2**19 + 2**5 + 2**2 + 2**1 + 1,
    2**20 + 2**3 + 1,
    2**21 + 2**2 + 1,
    2**22 + 2**1 + 1,
    2**23 + 2**5 + 1,
    2**24 + 2**4 + 2**3 + 2**1 + 1,
    2**25 + 2**3 + 1,
    2**26 + 2**4 + 2**3 + 2**1 + 1,
    2**27 + 2**5 + 2**2 + 2**1 + 1,
    2**28 + 2**1 + 1,
    2**29 + 2**2 + 1,
    2**30 + 2**1 + 1,
    2**31 + 2**3 + 1,
    2**32 + 2**7 + 2**3 + 2**2 + 1,
