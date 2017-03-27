# Hypergraphs based on generalized hyperedges

This library contains a C++ implementation of a generalized hypergraph in which edges can point to other edges.
The only entity here is the hyperedge; nodes are just special cases of hyperedges (0-Hyperedges).
A 1-hyperedge points to one element.
A 2-hyperedge can be seen as a standard undirected edge.

A graph in this sense is now just a collection of linked hyperedges.

## State

* Implemented basic hyperedge class
* Derive from basic hyperedge class the concept of relations (2-Hyperedges)
* From relations derive sets (memberOf relation)
* Derive from sets the concept of subsumption (is-a relation)
* Derive from sets the concept of composition (partOf relation). This is orthogonal to subsumption!
* Traversals use lambda functions for both result set filtering and traversal filtering
* Lambda function for decision on path is binary (needed for proper decision)
* Transitive closure of the already defined relations shall be defined somewhere

## TODO

* Make set and/or relation specific queries (e.g. on sets define transitive closure of memberOf relation - called members()?)
* Add more relations:
    - hasA to encode Aggregation
    - instanceOf to encode classes & objects (needed? typing is already there)
    - connects to encode Networks?
* When creating relations, it might be cool to check for already existing relations and update them accordingly if possible. This will make full use of the hypergraph system.
