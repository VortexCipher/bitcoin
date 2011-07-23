# FreeBSD Build Guide

**Updated for FreeBSD [14.0](https://www.freebsd.org/releases/14.0R/announce/)**

This guide describes how to build bitcoind, command-line utilities, and GUI on FreeBSD.

## Preparation

### 1. Install Required Dependencies
Run the following as root to install the base dependencies for building.

```bash
pkg install autoconf automake boost-libs git gmake libevent libtool pkgconf
