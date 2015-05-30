# Fuzzing Bitcoin Core using libFuzzer

## Quickstart guide

To quickly get started fuzzing Bitcoin Core using [libFuzzer](https://llvm.org/docs/LibFuzzer.html):

```sh
$ git clone https://github.com/bitcoin/bitcoin
$ cd bitcoin/
$ ./autogen.sh
$ CC=clang CXX=clang++ ./configure --enable-fuzz --with-sanitizers=address,fuzzer,undefined
# macOS users: If you have problem with this step then make sure to read "macOS hints for
# libFuzzer" on https://github.com/bitcoin/bitcoin/blob/master/doc/fuzzing.md#macos-hints-for-libfuzzer
$ make
$ FUZZ=process_message src/test/fuzz/fuzz
# abort fuzzing using ctrl-c
```

There is also a runner script to execute all fuzz targets. Refer to
`./test/fuzz/test_runner.py --help` for more details.

## Overview of Bitcoin Core fuzzing

[Google](https://github.com/google/fuzzing/) has a good overview of fuzzing in general, with contributions from key architects of some of the most-used fuzzers. [This paper](https://agroce.github.io/bitcoin_report.pdf) includes an external overview of the status of Bitcoin Core fuzzing, as of summer 2021.  [John Regehr](https://blog.regehr.org/archives/1687) provides good advice on writing code that assists fuzzers in finding bugs, which is useful for developers to keep in mind.

## Fuzzing harnesses and output

[`process_message`](https://github.com/bitcoin/bitcoin/blob/master/src/test/fuzz/process_message.cpp) is a fuzzing harness for the [`ProcessMessage(...)` function (`net_processing`)](https://github.com/bitcoin/bitcoin/blob/master/src/net_processing.cpp). The available fuzzing harnesses are found in [`src/test/fuzz/`](https://github.com/bitcoin/bitcoin/tree/master/src/test/fuzz).

The fuzzer will output `NEW` every time it has created a test input that covers new areas of the code under test. For more information on how to interpret the fuzzer output, see the [libFuzzer documentation](https://llvm.org/docs/LibFuzzer.html).

If you specify a corpus directory then any new coverage increasing inputs will be saved there:

```sh
