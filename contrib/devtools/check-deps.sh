#!/usr/bin/env bash

export LC_ALL=C
set -Eeuo pipefail

# Declare paths to libraries
declare -A LIBS
LIBS[cli]="libbitcoin_cli.a"
LIBS[common]="libbitcoin_common.a"
LIBS[consensus]="libbitcoin_consensus.a"
LIBS[crypto]="crypto/.libs/libbitcoin_crypto_base.a crypto/.libs/libbitcoin_crypto_x86_shani.a crypto/.libs/libbitcoin_crypto_sse41.a crypto/.libs/libbitcoin_crypto_avx2.a"
LIBS[node]="libbitcoin_node.a"
LIBS[util]="libbitcoin_util.a"
LIBS[wallet]="libbitcoin_wallet.a"
LIBS[wallet_tool]="libbitcoin_wallet_tool.a"

# Declare allowed dependencies "X Y" where X is allowed to depend on Y. This
# list is taken from doc/design/libraries.md.
ALLOWED_DEPENDENCIES=(
    "cli common"
