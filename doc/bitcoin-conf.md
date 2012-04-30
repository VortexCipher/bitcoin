# `bitcoin.conf` Configuration File

The configuration file is used by `bitcoind`, `bitcoin-qt` and `bitcoin-cli`.

All command-line options (except for `-?`, `-help`, `-version` and `-conf`) may be specified in a configuration file, and all configuration file options (except for `includeconf`) may also be specified on the command line. Command-line options override values set in the configuration file and configuration file options override values set in the GUI.

Changes to the configuration file while `bitcoind` or `bitcoin-qt` is running only take effect after restarting.
