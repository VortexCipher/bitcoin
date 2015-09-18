#!/usr/bin/env bash

# Based on realpath.sh written by Michael Kropat
# Found at: https://github.com/mkropat/sh-realpath/blob/65512368b8155b176b67122aa395ac580d9acc5b/realpath.sh

bash_realpath() {
    canonicalize_path "$(resolve_symlinks "$1")"
