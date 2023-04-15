// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <config/bitcoin-config.h> // IWYU pragma: keep

#include <randomenv.h>

#include <clientversion.h>
#include <compat/compat.h>
#include <compat/cpuid.h>
#include <crypto/sha512.h>
#include <span.h>
#include <support/cleanse.h>
#include <util/time.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <chrono>
#include <climits>
#include <thread>
#include <vector>

#include <sys/types.h> // must go before a number of other headers

#ifdef WIN32
#include <windows.h>
#include <winreg.h>
#else
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <unistd.h>
#endif
#if HAVE_DECL_GETIFADDRS && HAVE_DECL_FREEIFADDRS
#include <ifaddrs.h>
#endif
#ifdef HAVE_SYSCTL
#include <sys/sysctl.h>
#ifdef HAVE_VM_VM_PARAM_H
#include <vm/vm_param.h>
#endif
#ifdef HAVE_SYS_RESOURCES_H
#include <sys/resources.h>
