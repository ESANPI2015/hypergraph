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
* Although IS-CONCEPT and IS-RELATION are unary relations per definition, we use only two n-ary relations to represent them.
* Otherwise we would not be label-independent anymore and need to have multiple hedges encoding concepts and relations.
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
        Hyperedges concept(const UniqueId& id, const std::string& label="");                                                     //< Create a new concept (id, label)
        Hyperedges concepts(const std::string& label="") const;                                                                     //< Find a concept by label

        /* RELATIONS */
        Hyperedges relate(const UniqueId& id, const Hyperedges& fromIds, const Hyperedges& toIds, const std::string& label);    //< Create N:M relation
        Hyperedges relate(const Hyperedges& fromIds, const Hyperedges& toIds, const std::string& label);                        //< Create N:M relation using all info to generate a good UID
        Hyperedges relations(const std::string& label="") const;                                                                //< Find relations by label

        /* RELATIONS FROM A TEMPLATE */
        Hyperedges relateFrom(const UniqueId& id, const Hyperedges& fromIds, const Hyperedges& toIds, const UniqueId& relId);
        Hyperedges relateFrom(const Hyperedges& fromIds, const Hyperedges& toIds, const UniqueId& relId);                       //< Create another relation labelled like relId using all info to gen a good UID

        /* QUERIES */
        Hyperedges relationsFrom(const Hyperedges& ids, const std::string& label="") const;
        Hyperedges relationsTo(const Hyperedges& ids, const std::string& label="") const;
        Hyperedges relationsOf(const Hyperedges& ids, const std::string& label="") const
        {
            return unite(relationsFrom(ids,label), relationsTo(ids,label));
        }
        /* TRAVERSALS */
        // NOTE: ConceptFilterFunc signature: bool (const Conceptgraph& cg, const UniqueId& currentConcept, const Hyperedges& pathFromRootToConcept)
        // NOTE: RelationFilterFunc signature: bool (const Conceptgraph& cg, const UniqueId& currentConcept, const UniqueId& candidateRelation)
        template< typename ConceptFilterFunc, typename RelationFilterFunc > Hyperedges traverse(
                            const UniqueId& rootId,                                 //< Traverse the (sub)graph starting at rootId
                            ConceptFilterFunc cf,                                   //< visiting a concept OR relation this function should either return true or false.
                            RelationFilterFunc rf,                                  //< decide whether to follow a relation or not.
                            const TraversalDirection dir=FORWARD) const;
};

#include "Conceptgraph.tpp"

#endif
