# CJDNS support in Bitcoin Core

It is possible to run Bitcoin Core over CJDNS, an encrypted IPv6 network that
uses public-key cryptography for address allocation and a distributed hash table
for routing.

## What is CJDNS?

CJDNS is like a distributed, shared VPN with multiple entry points where every
participant can reach any other participant. All participants use addresses from
the `fc00::/8` network (reserved IPv6 range). Installation and configuration is
