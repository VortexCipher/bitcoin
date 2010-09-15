// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <rpc/request.h>

#include <common/args.h>
#include <logging.h>
#include <random.h>
#include <rpc/protocol.h>
#include <util/fs.h>
#include <util/fs_helpers.h>
#include <util/strencodings.h>

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * JSON-RPC protocol.  Bitcoin speaks version 1.0 for maximum compatibility,
 * but uses JSON-RPC 1.1/2.0 standards for parts of the 1.0 standard that were
 * unspecified (HTTP errors and contents of 'error').
