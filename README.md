# Hypergraphs based on generalized hyperedges

This library contains a C++ implementation of a generalized hypergraph in which edges can point to other edges.
The only entity here is the hyperedge; nodes could be modeled as 0-hyperedges but there is no extra entity.
The hyperedges are directed, that means that they have a set of other hyperedges FROM which they point and another set TO which they point.
A graph in this sense is now just a collection of linked hyperedges.
Some notation: (N,M)-Hyperedge means, that it points from N hyperedges to M other hyperedges
Note that the directed hyperedges can represent NxM normal, directed edges.
A possible extension would be to model hyperedges as being directed and oriented;
that means that they encode an ordered N-tuple of M-cardinal sets.

## State

* Implemented basic hyperedge class
* Traversals use lambda functions for both result set filtering and traversal filtering
* Derived from basic hyperedge class the concept of relations
* Derived from basic hyperedge class the concept of concepts
* Derived from relations a collection of common relations for semantic networks:
    - Subsumption through IS-A, SUBREL-OF
    - Aggregation through HAS-A
    - Composition through PART-OF
    - Individuals through INSTANCE-OF, Facts through FACT-OF
    - Topology through CONNECTS
* Transitive closure over the common relations through traversal
* Some basic queries and operations implemented
* Pattern matching algorithm according to Ullmann (find some and find another match)
* Rewrite algorithm with automatic node deletion and label transformation
* Easy chaining of queries possible because of signature harmonisation and overloaded ops

## TODO

* Rewrite algorithm does not yet create new nodes
