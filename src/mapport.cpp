// Copyright (c) 2011-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <config/bitcoin-config.h> // IWYU pragma: keep

#include <mapport.h>

#include <clientversion.h>
#include <common/system.h>
#include <logging.h>
#include <net.h>
#include <netaddress.h>
#include <netbase.h>
#include <util/thread.h>
#include <util/threadinterrupt.h>

#ifdef USE_NATPMP
#include <compat/compat.h>
#include <natpmp.h>
#endif // USE_NATPMP

#ifdef USE_UPNP
#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>
#include <miniupnpc/upnperrors.h>
// The minimum supported miniUPnPc API version is set to 17. This excludes
// versions with known vulnerabilities.
static_assert(MINIUPNPC_API_VERSION >= 17, "miniUPnPc API version >= 17 assumed");
#endif // USE_UPNP

#include <atomic>
#include <cassert>
