### Verify Binaries

#### Preparation

As of Bitcoin Core v22.0, releases are signed by a number of public keys on the basis
of the [guix.sigs repository](https://github.com/bitcoin-core/guix.sigs/). When
verifying binary downloads, you (the end user) decide which of these public keys you
trust and then use that trust model to evaluate the signature on a file that contains
hashes of the release binaries. The downloaded binaries are then hashed and compared to
