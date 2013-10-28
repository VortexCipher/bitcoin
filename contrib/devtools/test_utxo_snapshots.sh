#!/usr/bin/env bash
# Demonstrate the creation and usage of UTXO snapshots.
#
# A server node starts up, IBDs up to a certain height, then generates a UTXO
# snapshot at that point.
#
# The server then downloads more blocks (to create a diff from the snapshot).
#
# We bring a client up, load the UTXO snapshot, and we show the client sync to
# the "network tip" and then start a background validation of the snapshot it
# loaded. We see the background validation chainstate removed after validation
# completes.
#
# The shellcheck rule SC2086 (quoted variables) disablements are necessary
# since this rule needs to be violated in order to get bitcoind to pick up on
# $EARLY_IBD_FLAGS for the script to work.

export LC_ALL=C
set -e

