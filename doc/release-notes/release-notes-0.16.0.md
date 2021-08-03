Bitcoin Core version 0.16.0 is now available from:

  <https://bitcoincore.org/bin/bitcoin-core-0.16.0/>

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

The first time you run version 0.15.0 or newer, your chainstate database will be converted to a
new format, which will take anywhere from a few minutes to half an hour,
depending on the speed of your machine.

Note that the block database format also changed in version 0.8.0 and there is no
automatic upgrade code from before version 0.8 to version 0.15.0 or higher. Upgrading
directly from 0.7.x and earlier without re-downloading the blockchain is not supported.
However, as usual, old wallet versions are still supported.

Downgrading warning
-------------------

Wallets created in 0.16 and later are not compatible with versions prior to 0.16
and will not work if you try to use newly created wallets in older versions. Existing
wallets that were created with older versions are not affected by this.

Compatibility
==============

Bitcoin Core is extensively tested on multiple operating systems using
the Linux kernel, macOS 10.8+, and Windows Vista and later. Windows XP is not supported.

Bitcoin Core should also work on most other Unix-like systems but is not
frequently tested on them.

Notable changes
===============

Wallet changes
---------------

### Segwit Wallet

Bitcoin Core 0.16.0 introduces full support for segwit in the wallet and user interfaces. A new `-addresstype` argument has been added, which supports `legacy`, `p2sh-segwit` (default), and `bech32` addresses. It controls what kind of addresses are produced by `getnewaddress`, `getaccountaddress`, and `createmultisigaddress`. A `-changetype` argument has also been added, with the same options, and by default equal to `-addresstype`, to control which kind of change is used.

A new `address_type` parameter has been added to the `getnewaddress` and `addmultisigaddress` RPCs to specify which type of address to generate.
A `change_type` argument has been added to the `fundrawtransaction` RPC to override the `-changetype` argument for specific transactions.

- All segwit addresses created through `getnewaddress` or `*multisig` RPCs explicitly get their redeemscripts added to the wallet file. This means that downgrading after creating a segwit address will work, as long as the wallet file is up to date.
- All segwit keys in the wallet get an implicit redeemscript added, without it being written to the file. This means recovery of an old backup will work, as long as you use new software.
- All keypool keys that are seen used in transactions explicitly get their redeemscripts added to the wallet files. This means that downgrading after recovering from a backup that includes a segwit address will work

Note that some RPCs do not yet support segwit addresses. Notably, `signmessage`/`verifymessage` doesn't support segwit addresses, nor does `importmulti` at this time. Support for segwit in those RPCs will continue to be added in future versions.

P2WPKH change outputs are now used by default if any destination in the transaction is a P2WPKH or P2WSH output. This is done to ensure the change output is as indistinguishable from the other outputs as possible in either case.

### BIP173 (Bech32) Address support ("bc1..." addresses)

Full support for native segwit addresses (BIP173 / Bech32) has now been added.
