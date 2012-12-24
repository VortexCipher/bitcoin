# I2P support in Bitcoin Core

It is possible to run Bitcoin Core as an
[I2P (Invisible Internet Project)](https://en.wikipedia.org/wiki/I2P)
service and connect to such services.

This [glossary](https://geti2p.net/en/about/glossary) may be useful to get
started with I2P terminology.

## Run Bitcoin Core with an I2P router (proxy)

A running I2P router (proxy) is required with the [SAM](https://geti2p.net/en/docs/api/samv3)
application bridge enabled. The following routers are recommended for use with Bitcoin Core:

- [i2prouter (I2P Router)](https://geti2p.net), the official implementation in
  Java. The SAM bridge is not enabled by default; it must be started manually,
