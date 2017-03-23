# hypergraph

This library contains a C++ implementation of a generalized hypergraph in which edges can point to other edges.
The only entity here is the hyperedge; nodes are just labelled 0-hyperedges.
A 1-hyperedge points to one element.
A 2-hyperedge can be seen as a standard undirected edge.

A graph in this sense is now just a collection of hyperedges.

## State

* Implemented basic hyperedge class

## TODO

* Derive from basic hyperedge class the concept of relations (2-Hyperedges)
* From relations derive sets (memberOf relation)
* Derive from sets the concept of subsumption (is-a relation)
* Derive from sets the concept of conposition (partOf relation). This is orthogonal to subsumption!

