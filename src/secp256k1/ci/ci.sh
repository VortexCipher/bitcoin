#!/bin/sh

set -eux

export LC_ALL=C

# Print commit and relevant CI environment to allow reproducing the job outside of CI.
git show --no-patch
print_environment() {
    # Turn off -x because it messes up the output
    set +x
    # There are many ways to print variable names and their content. This one
    # does not rely on bash.
    for var in WERROR_CFLAGS MAKEFLAGS BUILD \
