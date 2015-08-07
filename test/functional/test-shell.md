Test Shell for Interactive Environments
=========================================

This document describes how to use the `TestShell` submodule in the functional
test suite.

The `TestShell` submodule extends the `BitcoinTestFramework` functionality to
external interactive environments for prototyping and educational purposes. Just
like `BitcoinTestFramework`, the `TestShell` allows the user to:

* Manage regtest bitcoind subprocesses.
* Access RPC interfaces of the underlying bitcoind instances.
* Log events to the functional test logging utility.

The `TestShell` can be useful in interactive environments where it is necessary
to extend the object lifetime of the underlying `BitcoinTestFramework` between
user inputs. Such environments include the Python3 command line interpreter or
[Jupyter](https://jupyter.org/) notebooks running a Python3 kernel.
