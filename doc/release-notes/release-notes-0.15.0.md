Bitcoin Core version *0.15.0* is now available from:

  <https://bitcoin.org/bin/bitcoin-core-0.15.0/>

This is a new major version release, including new features, various bugfixes
and performance improvements, as well as updated translations.

Please report bugs using the issue tracker at GitHub:

  <https://github.com/bitcoin/bitcoin/issues>

To receive security and update notifications, please subscribe to:

  <https://bitcoincore.org/en/list/announcements/join/>

How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes for older versions), then run the 
installer (on Windows) or just copy over `/Applications/Bitcoin-Qt` (on Mac)
or `bitcoind`/`bitcoin-qt` (on Linux).

The first time you run version 0.15.0, your chainstate database will be converted to a
new format, which will take anywhere from a few minutes to half an hour,
depending on the speed of your machine.

The file format of `fee_estimates.dat` changed in version 0.15.0. Hence, a
downgrade from version 0.15.0 or upgrade to version 0.15.0 will cause all fee
estimates to be discarded.

Note that the block database format also changed in version 0.8.0 and there is no
automatic upgrade code from before version 0.8 to version 0.15.0. Upgrading
directly from 0.7.x and earlier without redownloading the blockchain is not supported.
However, as usual, old wallet versions are still supported.

Downgrading warning
-------------------

The chainstate database for this release is not compatible with previous
releases, so if you run 0.15 and then decide to switch back to any
older version, you will need to run the old release with the `-reindex-chainstate`
option to rebuild the chainstate data structures in the old format.

If your node has pruning enabled, this will entail re-downloading and
processing the entire blockchain.

Compatibility
==============

Bitcoin Core is extensively tested on multiple operating systems using
the Linux kernel, macOS 10.8+, and Windows Vista and later. Windows XP is not supported.

Bitcoin Core should also work on most other Unix-like systems but is not
frequently tested on them.

Notes for 0.15.0
================

Current SegWit support
----------------------

Version 0.15.0 supports adding a segregated witness address via the `addwitnessaddress` RPC, but
please note that this is a testing/expert RPC, which does not guarantee recovery from backup. Only use
this RPC if you know what you are doing. More complete wallet support for segregated witness is coming
in a next version.

Rescanning with encrypted wallets
---------------------------------

As in previous versions, when using an encrypted HD wallet, the keypool cannot be topped up without unlocking
the wallet. This means that currently, in order to recover from a backup of an encrypted HD wallet, the user
must unlock the wallet with a really long timeout and manually trigger a rescan, otherwise they risk missing
some keys when auto-topup cannot run. Unfortunately there is no `rescan` RPC in this version, that will be
included in a future version, so for now a rescan can be triggered using one of the `import*` commands, using
a dummy address generated by another (trusted) wallet.

Notable changes
===============

Performance Improvements
------------------------

Version 0.15 contains a number of significant performance improvements, which make
Initial Block Download, startup, transaction and block validation much faster:

- The chainstate database (which is used for tracking UTXOs) has been changed
