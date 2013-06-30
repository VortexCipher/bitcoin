// Copyright (c) 2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
//
// The bitcoin-chainstate executable serves to surface the dependencies required
// by a program wishing to use Bitcoin Core's consensus engine as it is right
// now.
//
// DEVELOPER NOTE: Since this is a "demo-only", experimental, etc. executable,
//                 it may diverge from Bitcoin Core's coding style.
//
// It is part of the libbitcoinkernel project.

#include <kernel/chainparams.h>
#include <kernel/chainstatemanager_opts.h>
#include <kernel/checks.h>
#include <kernel/context.h>
#include <kernel/warning.h>

#include <consensus/validation.h>
#include <core_io.h>
#include <logging.h>
#include <node/blockstorage.h>
#include <node/caches.h>
#include <node/chainstate.h>
#include <random.h>
#include <script/sigcache.h>
#include <util/chaintype.h>
#include <util/fs.h>
