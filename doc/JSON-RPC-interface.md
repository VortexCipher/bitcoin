# JSON-RPC Interface

The headless daemon `bitcoind` has the JSON-RPC API enabled by default, the GUI
`bitcoin-qt` has it disabled by default. This can be changed with the `-server`
option. In the GUI it is possible to execute RPC methods in the Debug Console
Dialog.

## Endpoints

There are two JSON-RPC endpoints on the server:

1. `/`
2. `/wallet/<walletname>/`

### `/` endpoint

This endpoint is always active.
It can always service non-wallet requests and can service wallet requests when
exactly one wallet is loaded.

### `/wallet/<walletname>/` endpoint
