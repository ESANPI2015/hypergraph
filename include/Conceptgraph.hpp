#ifndef _CONCEPTGRAPH_HPP
#define _CONCEPTGRAPH_HPP

#include "Hypergraph.hpp"

/*
* A concept graph is a hypergraph in which some special entities exist.
* The hyperedge of ID 1 which point to another hyperedge which is arbitrarily labelled e.g. "*"
* result in "*" being added to the set of concepts
* The hyperedge of ID 2 points to hyperedges encoding relations.
* 
* This graph does not introduce concrete concepts or relations it just serves as the basis for doing so.
*
* There are possible other encodings: e.g. we could use FIXED IDs (UIDs) to encode the CONCEPT and RELATION master hedges.
* An advantage would be that this encoding would be natural language (e.g. english, french) independent.
*
* In order to enforce unique IDs, automatic ID assignment is not performed!
*
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

        // creates c.id and (C(id=1) -> c)
        bool create(const unsigned id, const std::string& label="");
        // Will used a hashed version of label as id
        unsigned create(const std::string& label);
        // Find all concepts with a given label (none given means returning all concepts)
        Hyperedges find(const std::string& label="");
        // creates (a --r.id--> b) and (R(id=2) -> r) and uses the label of defId
        bool relate(const unsigned id, const unsigned fromId, const unsigned toId, const std::string& label="");
        // Nice version for chaining with queries: This and the queries actually encode rules!
        bool relate(const unsigned id, const Hyperedges& fromIds, const Hyperedges& toIds, const std::string& label="");
        // Uses a hash of label, fromId and toId as own id
        unsigned relate(const unsigned fromId, const unsigned toId, const std::string& label="");
        unsigned relate(const Hyperedges& fromIds, const Hyperedges& toIds, const std::string& label="");
        // destroys either a concept or a relation (destruction of a concept will destroy relations as well)
        void     destroy(const unsigned id);


        // returns all relations R of which c is either in the from or the to set (and matching label with defId)
        Hyperedges relationsOf(const unsigned conceptId, const std::string& label="");
        // Nice version for chaining with find() and previous relationsOf()
        Hyperedges relationsOf(const Hyperedges& concepts, const std::string& label="");
        // Traverse the subgraph starting at root, filtering result by conceptLabel and only taking paths via relations labelled relationLabel
        // NOTE: This is very different from the Hypergraph::traversal!
        Hyperedges traverse(const unsigned rootId, const std::string& conceptLabel="", const std::string& relationLabel="", const TraversalDirection dir=DOWN);
};

#endif
