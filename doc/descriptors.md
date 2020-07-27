# Support for Output Descriptors in Bitcoin Core

Since Bitcoin Core v0.17, there is support for Output Descriptors. This is a
simple language which can be used to describe collections of output scripts.
Supporting RPCs are:
- `scantxoutset` takes as input descriptors to scan for, and also reports
  specialized descriptors for the matching UTXOs.
- `getdescriptorinfo` analyzes a descriptor, and reports a canonicalized version
  with checksum added.
- `deriveaddresses` takes as input a descriptor and computes the corresponding
  addresses.
- `listunspent` outputs a specialized descriptor for the reported unspent outputs.
- `getaddressinfo` outputs a descriptor for solvable addresses (since v0.18).
- `importmulti` takes as input descriptors to import into a legacy wallet
  (since v0.18).
- `generatetodescriptor` takes as input a descriptor and generates coins to it
  (`regtest` only, since v0.19).
- `utxoupdatepsbt` takes as input descriptors to add information to the psbt
  (since v0.19).
- `createmultisig` and `addmultisigaddress` return descriptors as well (since v0.20).
- `importdescriptors` takes as input descriptors to import into a descriptor wallet
  (since v0.21).
- `listdescriptors` outputs descriptors imported into a descriptor wallet (since v22).
- `scanblocks` takes as input descriptors to scan for in blocks and returns the
   relevant blockhashes (since v25).

This document describes the language. For the specifics on usage, see the RPC
documentation for the functions mentioned above.
