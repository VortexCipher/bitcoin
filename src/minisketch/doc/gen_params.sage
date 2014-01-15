#!/usr/bin/env sage
r"""
Generate finite field parameters for minisketch.

This script selects the finite fields used by minisketch
 for various sizes and generates the required tables for
 the implementation.

The output (after formatting) can be found in src/fields/*.cpp.

"""
B.<b> = GF(2)
P.<p> = B[]

def apply_map(m, v):
    r = 0
    i = 0
    while v != 0:
        if (v & 1):
            r ^^= m[i]
        i += 1
        v >>= 1
    return r

def recurse_moduli(acc, maxweight, maxdegree):
    for pos in range(maxweight, maxdegree + 1, 1):
        poly = acc + p^pos
        if maxweight == 1:
            if poly.is_irreducible():
                return (pos, poly)
        else:
            (deg, ret) = recurse_moduli(poly, maxweight - 1, pos - 1)
            if ret is not None:
