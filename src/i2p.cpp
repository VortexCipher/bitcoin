// Copyright (c) 2020-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparams.h>
#include <common/args.h>
#include <compat/compat.h>
#include <compat/endian.h>
#include <crypto/sha256.h>
#include <i2p.h>
#include <logging.h>
#include <netaddress.h>
#include <netbase.h>
#include <random.h>
#include <script/parsing.h>
#include <sync.h>
#include <tinyformat.h>
#include <util/fs.h>
#include <util/readwritefile.h>
#include <util/sock.h>
#include <util/strencodings.h>
#include <util/threadinterrupt.h>

#include <chrono>
#include <memory>
#include <stdexcept>
#include <string>

using util::Split;

namespace i2p {

/**
 * Swap Standard Base64 <-> I2P Base64.
 * Standard Base64 uses `+` and `/` as last two characters of its alphabet.
 * I2P Base64 uses `-` and `~` respectively.
 * So it is easy to detect in which one is the input and convert to the other.
 * @param[in] from Input to convert.
 * @return converted `from`
 */
static std::string SwapBase64(const std::string& from)
{
    std::string to;
    to.resize(from.size());
    for (size_t i = 0; i < from.size(); ++i) {
        switch (from[i]) {
        case '-':
            to[i] = '+';
            break;
