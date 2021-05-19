#!/usr/bin/env python3
# Copyright (c) 2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test Miniscript descriptors integration in the wallet."""

from test_framework.descriptors import descsum_create
from test_framework.psbt import PSBT, PSBT_IN_SHA256
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import assert_equal


TPRVS = [
    "tprv8ZgxMBicQKsPerQj6m35no46amfKQdjY7AhLnmatHYXs8S4MTgeZYkWAn4edSGwwL3vkSiiGqSZQrmy5D3P5gBoqgvYP2fCUpBwbKTMTAkL",
    "tprv8ZgxMBicQKsPd3cbrKjE5GKKJLDEidhtzSSmPVtSPyoHQGL2LZw49yt9foZsN9BeiC5VqRaESUSDV2PS9w7zAVBSK6EQH3CZW9sMKxSKDwD",
    "tprv8iF7W37EHnVEtDr9EFeyFjQJFL6SfGby2AnZ2vQARxTQHQXy9tdzZvBBVp8a19e5vXhskczLkJ1AZjqgScqWL4FpmXVp8LLjiorcrFK63Sr",
]
TPUBS = [
    "tpubD6NzVbkrYhZ4YPAbyf6urxqqnmJF79PzQtyERAmvkSVS9fweCTjxjDh22Z5St9fGb1a5DUCv8G27nYupKP1Ctr1pkamJossoetzws1moNRn",
    "tpubD6NzVbkrYhZ4YMQC15JS7QcrsAyfGrGiykweqMmPxTkEVScu7vCZLNpPXW1XphHwzsgmqdHWDQAfucbM72EEB1ZEyfgZxYvkZjYVXx1xS9p",
    "tpubD6NzVbkrYhZ4YU9vM1s53UhD75UyJatx8EMzMZ3VUjR2FciNfLLkAw6a4pWACChzobTseNqdWk4G7ZdBqRDLtLSACKykTScmqibb1ZrCvJu",
    "tpubD6NzVbkrYhZ4XRMcMFMMFvzVt6jaDAtjZhD7JLwdPdMm9xa76DnxYYP7w9TZGJDVFkek3ArwVsuacheqqPog8TH5iBCX1wuig8PLXim4n9a",
    "tpubD6NzVbkrYhZ4WsqRzDmkL82SWcu42JzUvKWzrJHQ8EC2vEHRHkXj1De93sD3biLrKd8XGnamXURGjMbYavbszVDXpjXV2cGUERucLJkE6cy",
    "tpubDEFLeBkKTm8aiYkySz8hXAXPVnPSfxMi7Fxhg9sejUrkwJuRWvPdLEiXjTDbhGbjLKCZUDUUibLxTnK5UP1q7qYrSnPqnNe7M8mvAW1STcc",
    "tpubD6NzVbkrYhZ4WR99ygpiJvPMAJiwahjLgGywc5vJx2gUfKUfEPCrbKmQczDPJZmLcyZzRb5Ti6rfUb89S2WFyPH7FDtD6RFDA1hdgTEgEUL",
]
PUBKEYS = [
    "02aebf2d10b040eb936a6f02f44ee82f8b34f5c1ccb20ff3949c2b28206b7c1068",
    "030f64b922aee2fd597f104bc6cb3b670f1ca2c6c49b1071a1a6c010575d94fe5a",
    "02abe475b199ec3d62fa576faee16a334fdb86ffb26dce75becebaaedf328ac3fe",
    "0314f3dc33595b0d016bb522f6fe3a67680723d842c1b9b8ae6b59fdd8ab5cccb4",
    "025eba3305bd3c829e4e1551aac7358e4178832c739e4fc4729effe428de0398ab",
    "029ffbe722b147f3035c87cb1c60b9a5947dd49c774cc31e94773478711a929ac0",
    "0211c7b2e18b6fd330f322de087da62da92ae2ae3d0b7cec7e616479cce175f183",
]

P2WSH_MINISCRIPTS = [
    # One of two keys
    f"or_b(pk({TPUBS[0]}/*),s:pk({TPUBS[1]}/*))",
    # A script similar (same spending policy) to BOLT3's offered HTLC (with anchor outputs)
