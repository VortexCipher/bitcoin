#!/usr/bin/env python3
# Copyright (c) 2014 Wladimir J. van der Laan
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
'''
A script to check that release executables only contain certain symbols
and are only linked against allowed libraries.

Example usage:

    find ../path/to/binaries -type f -executable | xargs python3 contrib/devtools/symbol-check.py
'''
import sys

import lief

# Debian 11 (Bullseye) EOL: 2026. https://wiki.debian.org/LTS
#
# - libgcc version 10.2.1 (https://packages.debian.org/bullseye/libgcc-s1)
# - libc version 2.31 (https://packages.debian.org/source/bullseye/glibc)
#
# Ubuntu 20.04 (Focal) EOL: 2030. https://wiki.ubuntu.com/ReleaseTeam
#
# - libgcc version 10.5.0 (https://packages.ubuntu.com/focal/libgcc1)
# - libc version 2.31 (https://packages.ubuntu.com/focal/libc6)
#
# CentOS Stream 9 EOL: 2027. https://www.centos.org/cl-vs-cs/#end-of-life
#
# - libgcc version 12.2.1 (https://mirror.stream.centos.org/9-stream/AppStream/x86_64/os/Packages/)
# - libc version 2.34 (https://mirror.stream.centos.org/9-stream/AppStream/x86_64/os/Packages/)
#
