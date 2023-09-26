# Bootstrappable Bitcoin Core Builds

This directory contains the files necessary to perform bootstrappable Bitcoin
Core builds.

[Bootstrappability][b17e] furthers our binary security guarantees by allowing us
to _audit and reproduce_ our toolchain instead of blindly _trusting_ binary
downloads.

We achieve bootstrappability by using Guix as a functional package manager.

# Requirements

Conservatively, you will need:

- 16GB of free disk space on the partition that /gnu/store will reside in
- 8GB of free disk space **per platform triple** you're planning on building
  (see the `HOSTS` [environment variable description][env-vars-list])

# Installation and Setup

If you don't have Guix installed and set up, please follow the instructions in
[INSTALL.md](./INSTALL.md)

# Usage

If you haven't considered your security model yet, please read [the relevant
section](#choosing-your-security-model) before proceeding to perform a build.

## Making the Xcode SDK available for macOS cross-compilation

In order to perform a build for macOS (which is included in the default set of
platform triples to build), you'll need to extract the macOS SDK tarball using
tools found in the [`macdeploy` directory](../macdeploy/README.md).

You can then either point to the SDK using the `SDK_PATH` environment variable:

```sh
# Extract the SDK tarball to /path/to/parent/dir/of/extracted/SDK/Xcode-<foo>-<bar>-extracted-SDK-with-libcxx-headers
tar -C /path/to/parent/dir/of/extracted/SDK -xaf /path/to/Xcode-<foo>-<bar>-extracted-SDK-with-libcxx-headers.tar.gz

# Indicate where to locate the SDK tarball
export SDK_PATH=/path/to/parent/dir/of/extracted/SDK
