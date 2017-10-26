UNIX BUILD NOTES
====================
Some notes on how to build Bitcoin Core in Unix.

(For BSD specific instructions, see `build-*bsd.md` in this directory.)

To Build
---------------------

```bash
./autogen.sh
./configure
make # use "-j N" for N parallel jobs
make install # optional
```

See below for instructions on how to [install the dependencies on popular Linux
distributions](#linux-distribution-specific-instructions), or the
[dependencies](#dependencies) section for a complete overview.

## Memory Requirements
