# TOR SUPPORT IN BITCOIN

It is possible to run Bitcoin Core as a Tor onion service, and connect to such services.

The following directions assume you have a Tor proxy running on port 9050. Many distributions default to having a SOCKS proxy listening on port 9050, but others may not. In particular, the Tor Browser Bundle defaults to listening on port 9150.
## Compatibility

- Starting with version 22.0, Bitcoin Core only supports Tor version 3 hidden
  services (Tor v3). Tor v2 addresses are ignored by Bitcoin Core and neither
  relayed nor stored.

- Tor removed v2 support beginning with version 0.4.6.

## How to see information about your Tor configuration via Bitcoin Core

There are several ways to see your local onion address in Bitcoin Core:
- in the "Local addresses" output of CLI `-netinfo`
- in the "localaddresses" output of RPC `getnetworkinfo`
- in the debug log (grep for "AddLocal"; the Tor address ends in `.onion`)

You may set the `-debug=tor` config logging option to have additional
