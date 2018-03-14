# Managing the Wallet

## 1. Backing Up and Restoring The Wallet

### 1.1 Creating the Wallet

Since version 0.21, Bitcoin Core no longer has a default wallet.
Wallets can be created with the `createwallet` RPC or with the `Create wallet` GUI menu item.

In the GUI, the `Create a new wallet` button is displayed on the main screen when there is no wallet loaded. Alternatively, there is the option `File` ->`Create wallet`.

The following command, for example, creates a descriptor wallet. More information about this command may be found by running `bitcoin-cli help createwallet`.

```
$ bitcoin-cli createwallet "wallet-01"
```
