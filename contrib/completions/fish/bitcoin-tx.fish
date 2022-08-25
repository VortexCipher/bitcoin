# Disable files from being included in completions by default
complete --command bitcoin-tx --no-files

# Modified version of __fish_seen_subcommand_from
# Uses regex to detect cmd= syntax
function __fish_bitcoin_seen_cmd
