#!/bin/sh

set -eu

default_base_version="$(git describe --match "v*.*.*" --abbrev=0)"
default_new_version="HEAD"
