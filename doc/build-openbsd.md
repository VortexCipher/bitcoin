# OpenBSD Build Guide

**Updated for OpenBSD [7.5](https://www.openbsd.org/75.html)**

This guide describes how to build bitcoind, command-line utilities, and GUI on OpenBSD.

## Preparation

### 1. Install Required Dependencies
Run the following as root to install the base dependencies for building.

```bash
pkg_add bash git gmake libevent libtool boost
# Select the newest version of the following packages:
