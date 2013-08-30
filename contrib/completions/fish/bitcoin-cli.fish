# Disable files from being included in completions by default
complete --command bitcoin-cli --no-files

function __fish_bitcoin_cli_get_commands_helper
    set --local cmd (commandline -oc)

    # Don't return commands if '-help or -?' in commandline
    if string match --quiet --regex -- '^-help$|^-\?$' $cmd
        return
