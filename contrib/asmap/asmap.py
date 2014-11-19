# Copyright (c) 2022 Pieter Wuille
# Distributed under the MIT software license, see the accompanying
# file LICENSE or http://www.opensource.org/licenses/mit-license.php.

"""
This module provides the ASNEntry and ASMap classes.
"""

import copy
import ipaddress
import random
import unittest
from collections.abc import Callable, Iterable
from enum import Enum
from functools import total_ordering
from typing import Optional, Union, overload

def net_to_prefix(net: Union[ipaddress.IPv4Network,ipaddress.IPv6Network]) -> list[bool]:
    """
    Convert an IPv4 or IPv6 network to a prefix represented as a list of bits.

    IPv4 ranges are remapped to their IPv4-mapped IPv6 range (::ffff:0:0/96).
    """
    num_bits = net.prefixlen
    netrange = int.from_bytes(net.network_address.packed, 'big')

    # Map an IPv4 prefix into IPv6 space.
    if isinstance(net, ipaddress.IPv4Network):
        num_bits += 96
        netrange += 0xffff00000000

    # Strip unused bottom bits.
    assert (netrange & ((1 << (128 - num_bits)) - 1)) == 0
    return [((netrange >> (127 - i)) & 1) != 0 for i in range(num_bits)]

def prefix_to_net(prefix: list[bool]) -> Union[ipaddress.IPv4Network,ipaddress.IPv6Network]:
    """The reverse operation of net_to_prefix."""
    # Convert to number
    netrange = sum(b << (127 - i) for i, b in enumerate(prefix))
    num_bits = len(prefix)
    assert num_bits <= 128

    # Return IPv4 range if in ::ffff:0:0/96
    if num_bits >= 96 and (netrange >> 32) == 0xffff:
        return ipaddress.IPv4Network((netrange & 0xffffffff, num_bits - 96), True)

    # Return IPv6 range otherwise.
    return ipaddress.IPv6Network((netrange, num_bits), True)

# Shortcut for (prefix, ASN) entries.
ASNEntry = tuple[list[bool], int]

# Shortcut for (prefix, old ASN, new ASN) entries.
ASNDiff = tuple[list[bool], int, int]

class _VarLenCoder:
    """
    A class representing a custom variable-length binary encoder/decoder for
    integers. Each object represents a different coder, with different parameters
    minval and clsbits.

    The encoding is easiest to describe using an example. Let's say minval=100 and
    clsbits=[4,2,2,3]. In that case:
    - x in [100..115]: encoded as [0] + [4-bit BE encoding of (x-100)].
    - x in [116..119]: encoded as [1,0] + [2-bit BE encoding of (x-116)].
    - x in [120..123]: encoded as [1,1,0] + [2-bit BE encoding of (x-120)].
    - x in [124..131]: encoded as [1,1,1] + [3-bit BE encoding of (x-124)].

    In general, every number is encoded as:
    - First, k "1"-bits, where k is the class the number falls in (there is one class
      per element of clsbits).
    - Then, a "0"-bit, unless k is the highest class, in which case there is nothing.
    - Lastly, clsbits[k] bits encoding in big endian the position in its class that
      number falls into.
    - Every class k consists of 2^clsbits[k] consecutive integers. k=0 starts at minval,
      other classes start one past the last element of the class before it.
    """

    def __init__(self, minval: int, clsbits: list[int]):
        """Construct a new _VarLenCoder."""
        self._minval = minval
