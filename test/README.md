This directory contains integration tests that test bitcoind and its
utilities in their entirety. It does not contain unit tests, which
can be found in [/src/test](/src/test), [/src/wallet/test](/src/wallet/test),
etc.

This directory contains the following sets of tests:

- [fuzz](/test/fuzz) A runner to execute all fuzz targets from
  [/src/test/fuzz](/src/test/fuzz).
- [functional](/test/functional) which test the functionality of
bitcoind and bitcoin-qt by interacting with them through the RPC and P2P
interfaces.
- [util](/test/util) which tests the utilities (bitcoin-util, bitcoin-tx, ...).
- [lint](/test/lint/) which perform various static analysis checks.

The util tests are run as part of `make check` target. The fuzz tests, functional
tests and lint scripts can be run as explained in the sections below.

# Running tests locally

Before tests can be run locally, Bitcoin Core must be built.  See the [building instructions](/doc#building) for help.

## Fuzz tests

See [/doc/fuzzing.md](/doc/fuzzing.md)

### Functional tests

#### Dependencies and prerequisites

The ZMQ functional test requires a python ZMQ library. To install it:

- on Unix, run `sudo apt-get install python3-zmq`
