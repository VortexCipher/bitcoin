# Offline Signing Tutorial

This tutorial will describe how to use two instances of Bitcoin Core, one online and one offline, to greatly increase security by not having private keys reside on a networked device.

Maintaining an air-gap between private keys and any network connections drastically reduces the opportunity for those keys to be exfiltrated from the user.

This workflow uses [Partially Signed Bitcoin Transactions](https://github.com/bitcoin/bitcoin/blob/master/doc/psbt.md) (PSBTs) to transfer the transaction to and from the offline wallet for signing using the private keys.

> [!NOTE]
> While this tutorial demonstrates the process using `signet` network, you should omit the `-signet` flag in the provided commands when working with `mainnet`.

## Overview
In this tutorial we have two hosts, both running Bitcoin v25.0

* `offline` host which is disconnected from all networks (internet, Tor, wifi, bluetooth etc.) and does not have, or need, a copy of the blockchain.
* `online` host which is a regular online node with a synced blockchain.

We are going to first create an `offline_wallet` on the offline host. We will then create a `watch_only_wallet` on the online host using public key descriptors exported from the `offline_wallet`. Next we will receive some coins into the wallet. In order to spend these coins we'll create an unsigned PSBT using the `watch_only_wallet`, sign the PSBT using the private keys in the `offline_wallet`, and finally broadcast the signed PSBT using the online host.

### Requirements
- [jq](https://jqlang.github.io/jq/) installation - This tutorial uses jq to process certain fields from JSON RPC responses, but this convenience is optional.

### Create and Prepare the `offline_wallet`

1. On the offline machine create a wallet named `offline_wallet` secured by a wallet `passphrase`. This wallet will contain private keys and must remain unconnected to any networks at all times.
