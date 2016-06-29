# bash programmable completion for bitcoin-cli(1)
# Copyright (c) 2012-2022 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

# call $bitcoin-cli for RPC
_bitcoin_rpc() {
    # determine already specified args necessary for RPC
    local rpcargs=()
    for i in ${COMP_LINE}; do
        case "$i" in
            -conf=*|-datadir=*|-regtest|-rpc*|-testnet|-testnet4)
                rpcargs=( "${rpcargs[@]}" "$i" )
                ;;
