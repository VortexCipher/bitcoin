# assumeutxo

Assumeutxo is a feature that allows fast bootstrapping of a validating bitcoind
instance.

## Loading a snapshot

There is currently no canonical source for snapshots, but any downloaded snapshot
will be checked against a hash that's been hardcoded in source code.

Once you've obtained the snapshot, you can use the RPC command `loadtxoutset` to
load it.

### Pruning

A pruned node can load a snapshot. To save space, it's possible to delete the
