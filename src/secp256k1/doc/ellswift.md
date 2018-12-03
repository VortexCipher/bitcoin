# ElligatorSwift for secp256k1 explained

In this document we explain how the `ellswift` module implementation is related to the
construction in the
["SwiftEC: Shallue–van de Woestijne Indifferentiable Function To Elliptic Curves"](https://eprint.iacr.org/2022/759)
paper by Jorge Chávez-Saab, Francisco Rodríguez-Henríquez, and Mehdi Tibouchi.

* [1. Introduction](#1-introduction)
* [2. The decoding function](#2-the-decoding-function)
  + [2.1 Decoding for `secp256k1`](#21-decoding-for-secp256k1)
* [3. The encoding function](#3-the-encoding-function)
  + [3.1 Switching to *v, w* coordinates](#31-switching-to-v-w-coordinates)
  + [3.2 Avoiding computing all inverses](#32-avoiding-computing-all-inverses)
  + [3.3 Finding the inverse](#33-finding-the-inverse)
  + [3.4 Dealing with special cases](#34-dealing-with-special-cases)
  + [3.5 Encoding for `secp256k1`](#35-encoding-for-secp256k1)
* [4. Encoding and decoding full *(x, y)* coordinates](#4-encoding-and-decoding-full-x-y-coordinates)
  + [4.1 Full *(x, y)* coordinates for `secp256k1`](#41-full-x-y-coordinates-for-secp256k1)

## 1. Introduction

The `ellswift` module effectively introduces a new 64-byte public key format, with the property
that (uniformly random) public keys can be encoded as 64-byte arrays which are computationally
indistinguishable from uniform byte arrays. The module provides functions to convert public keys
from and to this format, as well as convenience functions for key generation and ECDH that operate
directly on ellswift-encoded keys.

The encoding consists of the concatenation of two (32-byte big endian) encoded field elements $u$
and $t.$ Together they encode an x-coordinate on the curve $x$, or (see further) a full point $(x, y)$ on
the curve.

**Decoding** consists of decoding the field elements $u$ and $t$ (values above the field size $p$
are taken modulo $p$), and then evaluating $F_u(t)$, which for every $u$ and $t$ results in a valid
x-coordinate on the curve. The functions $F_u$ will be defined in [Section 2](#2-the-decoding-function).

**Encoding** a given $x$ coordinate is conceptually done as follows:
* Loop:
  * Pick a uniformly random field element $u.$
  * Compute the set $L = F_u^{-1}(x)$ of $t$ values for which $F_u(t) = x$, which may have up to *8* elements.
  * With probability $1 - \dfrac{\\#L}{8}$, restart the loop.
  * Select a uniformly random $t \in L$ and return $(u, t).$

This is the *ElligatorSwift* algorithm, here given for just x-coordinates. An extension to full
$(x, y)$ points will be given in [Section 4](#4-encoding-and-decoding-full-x-y-coordinates).
The algorithm finds a uniformly random $(u, t)$ among (almost all) those
for which $F_u(t) = x.$ Section 3.2 in the paper proves that the number of such encodings for
almost all x-coordinates on the curve (all but at most 39) is close to two times the field size
(specifically, it lies in the range $2q \pm (22\sqrt{q} + O(1))$, where $q$ is the size of the field).
