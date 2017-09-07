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
* NOTE:
* The introduction of the two URELEMENTS is a shortcut for the following, more exact terminology:
* X <- IS-RELATION
* Y <- IS-CONCEPT <- FACT-OF -> IS-CONCEPT* <- IS-RELATION
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
        Hyperedges create(const unsigned id, const std::string& label="");  //< Create a new concept (id, label)
        Hyperedges create(const std::string& label);                        //< Create a new concept (hash(label), label)
        Hyperedges find(const std::string& label="");                       //< Find a concept by label

        /* RELATIONS */
        Hyperedges relate(const unsigned id, const unsigned fromId, const unsigned toId, const std::string& label);            //< Create relation fromId -- id --> toId
        Hyperedges relate(const unsigned id, const Hyperedges& fromIds, const Hyperedges& toIds, const std::string& label);    //< Create N:M relation
        Hyperedges relate(const unsigned fromId, const unsigned toId, const std::string& label);                           //< Create relation using hash(concat(labels)) as id
        Hyperedges relate(const Hyperedges& fromIds, const Hyperedges& toIds, const std::string& label);
        Hyperedges relations(const std::string& label="");              //< Find relations by label
        //Hyperedges relations(const std::vector<std::string>& labels="");              //< Find relations matching one of the given labels
        
        /* QUERIES */
        Hyperedges relationsFrom(const unsigned id, const std::string& label="");   //< Find all occurences of (id <-- label)
        Hyperedges relationsFrom(const Hyperedges& ids, const std::string& label="");
        Hyperedges relationsTo(const unsigned id, const std::string& label="");     //< Find all occurences of (label --> id)
        Hyperedges relationsTo(const Hyperedges& ids, const std::string& label="");
        Hyperedges relationsOf(const unsigned id, const std::string& label="")      //< Find all occurences of (label --> id) || (id <-- label)
        {
            return unite(relationsFrom(id,label), relationsTo(id,label));
        }
        Hyperedges relationsOf(const Hyperedges& ids, const std::string& label="")
        {
            return unite(relationsFrom(ids,label), relationsTo(ids,label));
        }
        /* TRAVERSALS */
        Hyperedges traverse(const unsigned rootId,                                  //< Traverse the (sub)graph starting at rootId
                            const std::string& visitLabel="",                       //< filter visited relations OR concepts by this label
                            const std::string& relationLabel="",                    //< follow relations matching this label
                            const TraversalDirection dir=DOWN);
        Hyperedges traverse(const unsigned rootId,                                  //< Traverse the (sub)graph starting at rootId
                            const std::vector<std::string>& visitLabels,            //< visited relations OR concepts matching one of these labels will be in the results
                            const std::vector<std::string>& relationLabels,         //< follow relations matching one of these labels
                            const TraversalDirection dir=DOWN);

        /* RELATIONS FROM TEMPLATES */
        Hyperedges relate(const unsigned id, const unsigned fromId, const unsigned toId, const unsigned relId);   //< Create a relation using relId as template
        Hyperedges relate(const unsigned fromId, const unsigned toId, const unsigned relId);                  //< Create relation using hash(concat(labels)) as id
        Hyperedges relate(const unsigned id, const Hyperedges& fromIds, const Hyperedges& toIds, const unsigned relId);
        Hyperedges relate(const Hyperedges& fromIds, const Hyperedges& toIds, const unsigned relId);
};

#endif
