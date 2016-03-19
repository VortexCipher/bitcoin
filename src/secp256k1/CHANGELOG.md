# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.5.1] - 2024-08-01

#### Added
 - Added usage example for an ElligatorSwift key exchange.

#### Changed
 - The default size of the precomputed table for signing was changed from 22 KiB to 86 KiB. The size can be changed with the configure option `--ecmult-gen-kb` (`SECP256K1_ECMULT_GEN_KB` for CMake).
 - "auto" is no longer an accepted value for the `--with-ecmult-window` and `--with-ecmult-gen-kb` configure options (this also applies to  `SECP256K1_ECMULT_WINDOW_SIZE` and `SECP256K1_ECMULT_GEN_KB` in CMake). To achieve the same configuration as previously provided by the "auto" value, omit setting the configure option explicitly.
