# Package Mempool Accept

## Definitions

A **package** is an ordered list of transactions, representable by a connected Directed Acyclic
Graph (a directed edge exists between a transaction that spends the output of another transaction).

For every transaction `t` in a **topologically sorted** package, if any of its parents are present
in the package, they appear somewhere in the list before `t`.

A **child-with-unconfirmed-parents** package is a topologically sorted package that consists of
exactly one child and all of its unconfirmed parents (no other transactions may be present).
The last transaction in the package is the child, and its package can be canonically defined based
on the current state: each of its inputs must be available in the UTXO set as of the current chain
tip or some preceding transaction in the package.
