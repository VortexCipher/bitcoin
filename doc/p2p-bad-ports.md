When Bitcoin Core automatically opens outgoing P2P connections, it chooses
a peer (address and port) from its list of potential peers. This list is
populated with unchecked data gossiped over the P2P network by other peers.

A malicious actor may gossip an address:port where no Bitcoin node is listening,
or one where a service is listening that is not related to the Bitcoin network.
As a result, this service may occasionally get connection attempts from Bitcoin
nodes.

"Bad" ports are ones used by services which are usually not open to the public
and usually require authentication. A connection attempt (by Bitcoin Core,
