Bitcoin Core version 0.18.0 is now available from:

  <https://bitcoincore.org/bin/bitcoin-core-0.18.0/>

This is a new major version release, including new features, various bug
fixes and performance improvements, as well as updated translations.

Please report bugs using the issue tracker at GitHub:

  <https://github.com/bitcoin/bitcoin/issues>

To receive security and update notifications, please subscribe to:

  <https://bitcoincore.org/en/list/announcements/join/>

How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has
completely shut down (which might take a few minutes for older
versions), then run the installer (on Windows) or just copy over
`/Applications/Bitcoin-Qt` (on Mac) or `bitcoind`/`bitcoin-qt` (on
Linux).

The first time you run version 0.15.0 or newer, your chainstate database
will be converted to a new format, which will take anywhere from a few
minutes to half an hour, depending on the speed of your machine.

Note that the block database format also changed in version 0.8.0 and
there is no automatic upgrade code from before version 0.8 to version
0.15.0 or later. Upgrading directly from 0.7.x and earlier without
redownloading the blockchain is not supported.  However, as usual, old
wallet versions are still supported.

Compatibility
==============

Bitcoin Core is supported and extensively tested on operating systems
using the Linux kernel, macOS 10.10+, and Windows 7 and newer. It is not
recommended to use Bitcoin Core on unsupported systems.

Bitcoin Core should also work on most other Unix-like systems but is not
as frequently tested on them.

From 0.17.0 onwards, macOS <10.10 is no longer supported. 0.17.0 is
built using Qt 5.9.x, which doesn't support versions of macOS older than
10.10. Additionally, Bitcoin Core does not yet change appearance when
macOS "dark mode" is activated.

In addition to previously-supported CPU platforms, this release's
pre-compiled distribution also provides binaries for the RISC-V
platform.

If you are using the `systemd` unit configuration file located at
`contrib/init/bitcoind.service`, it has been changed to use
`/var/lib/bitcoind` as the data directory instead of
`~bitcoin/.bitcoin`. When switching over to the new configuration file,
please make sure that the filesystem on which `/var/lib/bitcoind` will
exist has enough space (check using `df -h /var/lib/bitcoind`), and
optionally copy over your existing data directory. See the [systemd init
file section](#systemd-init-file) for more details.

Known issues
============

Wallet GUI
----------

For advanced users who have both (1) enabled coin control features, and
(2) are using multiple wallets loaded at the same time: The coin control
input selection dialog can erroneously retain wrong-wallet state when
switching wallets using the dropdown menu. For now, it is recommended
not to use coin control features with multiple wallets loaded.

Notable changes
===============

Mining
------

- Calls to `getblocktemplate` will fail if the segwit rule is not
  specified.  Calling `getblocktemplate` without segwit specified is
  almost certainly a misconfiguration since doing so results in lower
  rewards for the miner.  Failed calls will produce an error message
  describing how to enable the segwit rule.

Configuration option changes
----------------------------

- A warning is printed if an unrecognized section name is used in the
  configuration file.  Recognized sections are `[test]`, `[main]`, and
  `[regtest]`.

- Four new options are available for configuring the maximum number of
  messages that ZMQ will queue in memory (the "high water mark") before
  dropping additional messages.  The default value is 1,000, the same as
  was used for previous releases.  See the [ZMQ
  documentation](https://github.com/bitcoin/bitcoin/blob/master/doc/zmq.md#usage)
  for details.

- The `rpcallowip` option can no longer be used to automatically listen
  on all network interfaces.  Instead, the `rpcbind` parameter must be
  used to specify the IP addresses to listen on.  Listening for RPC
  commands over a public network connection is insecure and should be
  disabled, so a warning is now printed if a user selects such a
  configuration.  If you need to expose RPC in order to use a tool like
  Docker, ensure you only bind RPC to your localhost, e.g. `docker run
  [...] -p 127.0.0.1:8332:8332` (this is an extra `:8332` over the
  normal Docker port specification).

- The `rpcpassword` option now causes a startup error if the password
  set in the configuration file contains a hash character (#), as it's
  ambiguous whether the hash character is meant for the password or as a
  comment.

- The `whitelistforcerelay` option is used to relay transactions from
  whitelisted peers even when not accepted to the mempool. This option
  now defaults to being off, so that changes in policy and
  disconnect/ban behavior will not cause a node that is whitelisting
  another to be dropped by peers.  Users can still explicitly enable
  this behavior with the command line option (and may want to consider
  [contacting](https://bitcoincore.org/en/contact/) the Bitcoin Core
