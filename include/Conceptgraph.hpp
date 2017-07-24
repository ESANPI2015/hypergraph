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
        static const unsigned ConceptId;                                //< ID for identifying concept encoding hedge
        static const unsigned RelationId;                               //< ID for identifying relation encoding hedge

        /* Constructors/Destructors */
        Conceptgraph();
        Conceptgraph(Hypergraph& A);
        void     destroy(const unsigned id);                            //< Destroy concept, relation or signature

        /* CONCEPTS */
        bool create(const unsigned id, const std::string& label="");    //< Create a new concept (id, label)
        unsigned create(const std::string& label);                      //< Create a new concept (hash(label), label)
        Hyperedges find(const std::string& label="");                   //< Find a concept by label

        /* RELATIONS */
        bool relate(const unsigned id, const unsigned fromId, const unsigned toId, const std::string& label);            //< Create relation fromId -- id --> toId
        bool relate(const unsigned id, const Hyperedges& fromIds, const Hyperedges& toIds, const std::string& label);    //< Create N:M relation
        unsigned relate(const unsigned fromId, const unsigned toId, const std::string& label);                           //< Create relation using hash(concat(labels)) as id
        unsigned relate(const Hyperedges& fromIds, const Hyperedges& toIds, const std::string& label);
        
        /* QUERIES */
        Hyperedges relationsOf(const unsigned conceptId, const std::string& label="");                  //< Find all occurences of (label --> conceptId) || (conceptId <-- label)
        Hyperedges relationsOf(const Hyperedges& concepts, const std::string& label="");                //< Search over multiple concepts
        Hyperedges traverse(const unsigned rootId, const std::string& conceptLabel="",                  //< Traverse the (sub)graph starting at rootId
                            const std::string& relationLabel="", const TraversalDirection dir=DOWN);    //< filtering visited concepts/following resolved relations by label

        /* RELATIONS FROM TEMPLATES */
        bool relate(const unsigned id, const unsigned fromId, const unsigned toId, const unsigned relId);   //< Create a relation using relId as template
        unsigned relate(const unsigned fromId, const unsigned toId, const unsigned relId);                  //< Create relation using hash(concat(labels)) as id
        bool relate(const unsigned id, const Hyperedges& fromIds, const Hyperedges& toIds, const unsigned relId);
        unsigned relate(const Hyperedges& fromIds, const Hyperedges& toIds, const unsigned relId);
};

#endif
