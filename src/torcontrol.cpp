// Copyright (c) 2015-2022 The Bitcoin Core developers
// Copyright (c) 2017 The Zcash developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <torcontrol.h>

#include <chainparams.h>
#include <chainparamsbase.h>
#include <common/args.h>
#include <compat/compat.h>
#include <crypto/hmac_sha256.h>
#include <logging.h>
#include <net.h>
#include <netaddress.h>
#include <netbase.h>
#include <random.h>
#include <tinyformat.h>
#include <util/check.h>
#include <util/fs.h>
#include <util/readwritefile.h>
#include <util/strencodings.h>
#include <util/string.h>
#include <util/thread.h>
#include <util/time.h>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <deque>
#include <functional>
#include <map>
#include <optional>
#include <set>
#include <thread>
#include <utility>
#include <vector>

#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <event2/thread.h>
#include <event2/util.h>

using util::ReplaceAll;
using util::SplitString;
using util::ToString;

/** Default control ip and port */
const std::string DEFAULT_TOR_CONTROL = "127.0.0.1:" + ToString(DEFAULT_TOR_CONTROL_PORT);
/** Tor cookie size (from control-spec.txt) */
static const int TOR_COOKIE_SIZE = 32;
/** Size of client/server nonce for SAFECOOKIE */
static const int TOR_NONCE_SIZE = 32;
/** For computing serverHash in SAFECOOKIE */
static const std::string TOR_SAFE_SERVERKEY = "Tor safe cookie authentication server-to-controller hash";
/** For computing clientHash in SAFECOOKIE */
static const std::string TOR_SAFE_CLIENTKEY = "Tor safe cookie authentication controller-to-server hash";
/** Exponential backoff configuration - initial timeout in seconds */
static const float RECONNECT_TIMEOUT_START = 1.0;
/** Exponential backoff configuration - growth factor */
static const float RECONNECT_TIMEOUT_EXP = 1.5;
/** Maximum length for lines received on TorControlConnection.
 * tor-control-spec.txt mentions that there is explicitly no limit defined to line length,
 * this is belt-and-suspenders sanity limit to prevent memory exhaustion.
 */
static const int MAX_LINE_LENGTH = 100000;
static const uint16_t DEFAULT_TOR_SOCKS_PORT = 9050;

/****** Low-level TorControlConnection ********/

