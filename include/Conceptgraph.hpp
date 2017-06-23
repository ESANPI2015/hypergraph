#ifndef _CONCEPTGRAPH_HPP
#define _CONCEPTGRAPH_HPP

#include "Hypergraph.hpp"

/*
* A concept graph is a hypergraph in which some special entities exist.
* All hyperedges labelled "CONCEPT" which point to another hyperedge which is arbitrarily labelled e.g. "*"
* result in "*" being added to the set of concepts
* 
* This graph does not introduce concrete concepts or relations it just serves as the basis for doing so.
*
* There are possible other encodings: e.g. we could use FIXED IDs (UIDs) to encode the CONCEPT and RELATION master hedges.
* An advantage would be that this encoding would be natural language (e.g. english, french) independent.
*/

class Conceptgraph : public Hypergraph
{
    public:
        // IDs which will identify the HEDGES encoding other HEDGES as either being a CONCEPT or a RELATION
        static const unsigned ConceptId;
        static const unsigned RelationId;

        // Constructors
        Conceptgraph();
        // constructs a concept graph from a hypergraph
        Conceptgraph(Hypergraph& A);

        // creates c and (C.Id -> c)
        unsigned create(const std::string& label="");
        // Find all concepts with a given label (none given means returning all concepts)
        Hyperedges find(const std::string& label="");
        // creates (a --R--> b) and (R.Id -> R)
        unsigned relate(const unsigned fromId, const unsigned toId, const std::string& label="");
        // Nice version for chaining with queries: This and the queries actually encode rules!
        unsigned relate(const Hyperedges& fromIds, const Hyperedges& toIds, const std::string& label="");
        // destroys either a concept or a relation (destruction of a concept will destroy relations as well)
        void     destroy(const unsigned id);


        // returns all relations R of which c is either in the from or the to set (and having a certain label)
        Hyperedges relationsOf(const unsigned conceptId, const std::string& relationLabel="");
        // Nice version for chaining with find() and previous relationsOf()
        Hyperedges relationsOf(const Hyperedges& concepts, const std::string& relationLabel="");

        // Traverse the subgraph starting at root, filtering result by conceptLabel and only taking paths via relations labelled relationLabel
        // NOTE: This is very different from the Hypergraph::traversal!
        Hyperedges traverse(const unsigned rootId, const std::string& conceptLabel="", const std::string& relationLabel="", const TraversalDirection dir=DOWN);

        // Graph rewriting: It creates a new concept graph out of an old one by transforming concepts labelled <oldLabel> to <newLabel>
        //Conceptgraph(ConceptGraph& A, const std::string& oldLabel, const std::string& newLabel);

    protected:
        // two sets which hold either concept hedges or relation hedges (this is a cache)
        Hyperedges _concepts;
        Hyperedges _relations;
};

#endif
