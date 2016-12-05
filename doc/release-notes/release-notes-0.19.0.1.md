Bitcoin Core version 0.19.0.1 is now available from:

  <https://bitcoincore.org/bin/bitcoin-core-0.19.0.1/>

This release includes new features, various bug fixes and performance
improvements, as well as updated translations.

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

Upgrading directly from a version of Bitcoin Core that has reached its EOL is
possible, but might take some time if the datadir needs to be migrated.  Old
wallet versions of Bitcoin Core are generally supported.

Compatibility
==============

Bitcoin Core is supported and extensively tested on operating systems using
the Linux kernel, macOS 10.10+, and Windows 7 and newer. It is not recommended
to use Bitcoin Core on unsupported systems.

Bitcoin Core should also work on most other Unix-like systems but is not
as frequently tested on them.

From 0.17.0 onwards, macOS <10.10 is no longer supported. 0.17.0 is
built using Qt 5.9.x, which doesn't support versions of macOS older than
10.10. Additionally, Bitcoin Core does not yet change appearance when
macOS "dark mode" is activated.

Users running macOS Catalina may need to "right-click" and then choose "Open"
to open the Bitcoin Core .dmg. This is due to new signing requirements
imposed by Apple, which the Bitcoin Core project does not yet adhere too.

Notable changes
===============

New user documentation
----------------------

- [Reduce memory](https://github.com/bitcoin/bitcoin/blob/master/doc/reduce-memory.md)
  suggests configuration tweaks for running Bitcoin Core on systems with
  limited memory. (#16339)

New RPCs
--------

- `getbalances` returns an object with all balances (`mine`,
  `untrusted_pending` and `immature`). Please refer to the RPC help of
  `getbalances` for details. The new RPC is intended to replace
  `getbalance`, `getunconfirmedbalance`, and the balance fields in
  `getwalletinfo`.  These old calls and fields may be removed in a
  future version. (#15930, #16239)

- `setwalletflag` sets and unsets wallet flags that enable or disable
  features specific to that existing wallet, such as the new
  `avoid_reuse` feature documented elsewhere in these release notes.
  (#13756)

- `getblockfilter` gets the BIP158 filter for the specified block.  This
  RPC is only enabled if block filters have been created using the
  `-blockfilterindex` configuration option. (#14121)

New settings
------------

- `-blockfilterindex` enables the creation of BIP158 block filters for
  the entire blockchain.  Filters will be created in the background and
  currently use about 4 GiB of space.  Note: this version of Bitcoin
  Core does not serve block filters over the P2P network, although the
  local user may obtain block filters using the `getblockfilter` RPC.
  (#14121)

Updated settings
----------------

- `whitebind` and `whitelist` now accept a list of permissions to
  provide peers connecting using the indicated interfaces or IP
  addresses.  If no permissions are specified with an address or CIDR
  network, the implicit default permissions are the same as previous
  releases.  See the `bitcoind -help` output for these two options for
  details about the available permissions. (#16248)

- Users setting custom `dbcache` values can increase their setting slightly
  without using any more real memory.  Recent changes reduced the memory use
  by about 9% and made chainstate accounting more accurate (it was underestimating
  the use of memory before).  For example, if you set a value of "450" before, you
  may now set a value of "500" to use about the same real amount of memory. (#16957)


Updated RPCs
------------

Note: some low-level RPC changes mainly useful for testing are described in the
Low-level Changes section below.

