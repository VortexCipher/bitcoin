# PSBT Howto for Bitcoin Core

Since Bitcoin Core 0.17, an RPC interface exists for Partially Signed Bitcoin
Transactions (PSBTs, as specified in
[BIP 174](https://github.com/bitcoin/bips/blob/master/bip-0174.mediawiki)).

This document describes the overall workflow for producing signed transactions
through the use of PSBT, and the specific RPC commands used in typical
scenarios.

## PSBT in general

PSBT is an interchange format for Bitcoin transactions that are not fully signed
yet, together with relevant metadata to help entities work towards signing it.
