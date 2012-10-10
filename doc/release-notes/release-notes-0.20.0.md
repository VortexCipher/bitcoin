0.20.0 Release Notes
====================

Bitcoin Core version 0.20.0 is now available from:

  <https://bitcoincore.org/bin/bitcoin-core-0.20.0/>

This release includes new features, various bug fixes and performance
improvements, as well as updated translations.

Please report bugs using the issue tracker at GitHub:

  <https://github.com/bitcoin/bitcoin/issues>

To receive security and update notifications, please subscribe to:

  <https://bitcoincore.org/en/list/announcements/join/>

How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes in some cases), then run the
installer (on Windows) or just copy over `/Applications/Bitcoin-Qt` (on Mac)
or `bitcoind`/`bitcoin-qt` (on Linux).

Upgrading directly from a version of Bitcoin Core that has reached its EOL is
possible, but it might take some time if the data directory needs to be migrated. Old
wallet versions of Bitcoin Core are generally supported.

Compatibility
==============

Bitcoin Core is supported and extensively tested on operating systems
using the Linux kernel, macOS 10.12+, and Windows 7 and newer.  Bitcoin
Core should also work on most other Unix-like systems but is not as
frequently tested on them.  It is not recommended to use Bitcoin Core on
unsupported systems.

From Bitcoin Core 0.20.0 onwards, macOS versions earlier than 10.12 are no
longer supported. Additionally, Bitcoin Core does not yet change appearance
when macOS "dark mode" is activated.

Known Bugs
==========

The process for generating the source code release ("tarball") has changed in an
effort to make it more complete, however, there are a few regressions in
this release:

- The generated `configure` script is currently missing, and you will need to
  install autotools and run `./autogen.sh` before you can run
  `./configure`. This is the same as when checking out from git.

- Instead of running `make` simply, you should instead run
  `BITCOIN_GENBUILD_NO_GIT=1 make`.

Notable changes
===============

P2P and network changes
-----------------------

#### Removal of BIP61 reject network messages from Bitcoin Core

The `-enablebip61` command line option to enable BIP61 has been removed.
(#17004)

This feature has been disabled by default since Bitcoin Core version 0.18.0.
Nodes on the network can not generally be trusted to send valid messages
(including reject messages), so this should only ever be used when
connected to a trusted node.  Please use the alternatives recommended
below if you rely on this removed feature:

- Testing or debugging of implementations of the Bitcoin P2P network protocol
  should be done by inspecting the log messages that are produced by a recent
  version of Bitcoin Core. Bitcoin Core logs debug messages
  (`-debug=<category>`) to a stream (`-printtoconsole`) or to a file
  (`-debuglogfile=<debug.log>`).

- Testing the validity of a block can be achieved by specific RPCs:

  - `submitblock`

  - `getblocktemplate` with `'mode'` set to `'proposal'` for blocks with
    potentially invalid POW

- Testing the validity of a transaction can be achieved by specific RPCs:

  - `sendrawtransaction`

  - `testmempoolaccept`

- Wallets should not assume a transaction has propagated to the network
  just because there are no reject messages.  Instead, listen for the
  transaction to be announced by other peers on the network.  Wallets
  should not assume a lack of reject messages means a transaction pays
  an appropriate fee.  Instead, set fees using fee estimation and use
