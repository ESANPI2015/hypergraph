#ifndef _CONCEPTGRAPH_HPP
#define _CONCEPTGRAPH_HPP

#include "Hypergraph.hpp"

/*
* A concept graph is a hypergraph in which some special entities exist.
* All hyperedges labelled "CONCEPT" which point to another hyperedge which is arbitrarily labelled e.g. "*"
* result in "*" being added to the set of concepts
* 
* This graph does not introduce concrete concepts or relations it just serves as the basis for doing so.
*/

class Conceptgraph : public Hypergraph
{
    public:
        // Labels for identifiing concepts or relations
        static const std::string ConceptLabel;
        static const std::string RelationLabel;

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
        // destroys either a concept or a relation (destruction of a concept will destroy relations as well)
        void     destroy(const unsigned id);


        // returns all relations R of which c is either in the from or the to set (and having a certain label)
        Hyperedges relationsOf(const unsigned conceptId, const std::string& relationLabel="");
        // creates the transitive closure of a relation (which (probably) creates a new relation!)
        unsigned transitiveClosure(const unsigned root, const std::string& relationLabel="");

    protected:
        // get concept and relation candidates
        unsigned getConceptHyperedge();
        unsigned getRelationHyperedge();

        // two sets which hold either concept hedges or relation hedges (this is a cache)
        Hyperedges _concepts;
        Hyperedges _relations;
};

#endif
