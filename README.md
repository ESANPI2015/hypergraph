# hypergraph

This library contains a C++ implementation of a generalized hypergraph in which edges can point to other edges.
The only entity here is the hyperedge; nodes are just 0-hyperedges - named empty sets.
A 1-hyperedge is a set with one element.
A 2-hyperedge can be seen as the standard undirected edge.

A graph in this sense is now just a collection of hyperedges.

## State

* Implemented Hyperedge class
* Implement Hypergraph Import and Export (see serializer/deserializer)

## TODO

