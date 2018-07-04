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
* X <- IS-RELATION <- FACT-OF -> IS-RELATION*
* Y <- IS-CONCEPT <- FACT-OF -> IS-CONCEPT* <- IS-RELATION <- ...
*
* For now, we use the following to encode concepts and relations:
* X <- IS-RELATION to encode relations
* Y <- IS-CONCEPT to encode concepts
*
* The IS-CONCEPT relation is pointed by the IS-RELATION meta-relation.
*
*/

class Conceptgraph : public Hypergraph
{
    public:
        static const UniqueId IsConceptId;                                                                                      //< ID for identifying concept encoding hedge
        static const UniqueId IsRelationId;                                                                                     //< ID for identifying relation encoding hedge

        /* Constructors/Destructors */
        Conceptgraph();
        Conceptgraph(const Hypergraph& A);
        void createFundamentals();                                                                                              //< Creates the fundamental Hyperedges IS-RELATION and IS-CONCEPT
        void destroy(const UniqueId& id);                                                                                       //< Destroy concept, relation or signature

        /* CONCEPTS */
        Hyperedges create(const UniqueId& id, const std::string& label="");                                                     //< Create a new concept (id, label)
        Hyperedges find(const std::string& label="");                                                                           //< Find a concept by label

        /* RELATIONS */
        Hyperedges relate(const UniqueId& id, const Hyperedges& fromIds, const Hyperedges& toIds, const std::string& label);    //< Create N:M relation
        Hyperedges relate(const Hyperedges& fromIds, const Hyperedges& toIds, const std::string& label);                        //< Create N:M relation using all info to generate a good UID
        Hyperedges relations(const std::string& label="");                                                                      //< Find relations by label
        //Hyperedges relations(const std::vector<std::string>& labels="");              //< Find relations matching one of the given labels

        /* RELATIONS FROM A TEMPLATE */
        Hyperedges relateFrom(const UniqueId& id, const Hyperedges& fromIds, const Hyperedges& toIds, const UniqueId& relId);
        Hyperedges relateFrom(const Hyperedges& fromIds, const Hyperedges& toIds, const UniqueId& relId);                       //< Create another relation labelled like relId using all info to gen a good UID

        /* QUERIES */
        Hyperedges relationsFrom(const Hyperedges& ids, const std::string& label="");
        Hyperedges relationsTo(const Hyperedges& ids, const std::string& label="");
        Hyperedges relationsOf(const Hyperedges& ids, const std::string& label="")
        {
            return unite(relationsFrom(ids,label), relationsTo(ids,label));
        }
        /* TRAVERSALS */
        // TODO: Make these template functions?
        Hyperedges traverse(const UniqueId& rootId,                                 //< Traverse the (sub)graph starting at rootId
                            const std::string& visitLabel="",                       //< filter visited relations OR concepts by this label
                            const std::string& relationLabel="",                    //< follow relations matching this label
                            const TraversalDirection dir=FORWARD);
        Hyperedges traverse(const UniqueId& rootId,                                 //< Traverse the (sub)graph starting at rootId
                            const std::vector<std::string>& visitLabels,            //< visited relations OR concepts matching one of these labels will be in the results
                            const std::vector<std::string>& relationLabels,         //< follow relations matching one of these labels
                            const TraversalDirection dir=FORWARD);
};

#endif
