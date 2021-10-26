libsecp256k1
============

![Dependencies: None](https://img.shields.io/badge/dependencies-none-success)
[![irc.libera.chat #secp256k1](https://img.shields.io/badge/irc.libera.chat-%23secp256k1-success)](https://web.libera.chat/#secp256k1)

High-performance high-assurance C library for digital signatures and other cryptographic primitives on the secp256k1 elliptic curve.

This library is intended to be the highest quality publicly available library for cryptography on the secp256k1 curve. However, the primary focus of its development has been for usage in the Bitcoin system and usage unlike Bitcoin's may be less well tested, verified, or suffer from a less well thought out interface. Correct usage requires some care and consideration that the library is fit for your application's purpose.

Features:
* secp256k1 ECDSA signing/verification and key generation.
* Additive and multiplicative tweaking of secret/public keys.
* Serialization/parsing of secret keys, public keys, signatures.
