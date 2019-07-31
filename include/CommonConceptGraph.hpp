#ifndef _COMMON_CONCEPTGRAPH_HPP
#define _COMMON_CONCEPTGRAPH_HPP

#include "Conceptgraph.hpp"

/*
* This concept graph introduces the well known concepts of subsumption, composition etc.
* The following relation ensure a sound architecture of these concepts.
*
* SUBREL-OF     Encodes that some relation is a subrelation of some relation superclass (subsumption)
* IS-A          Encodes that some concept is a subclass of some concept class (subsumption)
* INSTANCE-OF   Encodes a concept being an instance of a concept class
* HAS-A         Encodes aggregation
* PART-OF       Encodes composition
* CONNECTS      Encodes topology
*
* To support the concept of relation classes and relation instances the following meta-relation is needed.
* However, it can not be easily encoded as the ones before, because it is somewhat ouside the simple relation world.
*
* FACT-OF       Encodes a relation being an instance of a relation class
*
* Consider now the following:
*
* FACT-OF <-- FACT-OF --> C[FACT-OF]
*             ^-- FACT-OF --^   ^
*                 ^-- FACT-OF --|
*
* Fortunately, we can encode this infinite loop! We do this:
*
* FACT-OF <-- FACT-OF --> C[FACT-OF]
*             ^--|
*
* That means that the POINTING FROM set of the FACT-OF in the middle points to itself!!
* Only ONE C[FACT-OF] and ONE FACT-OF with a loop is needed to encode all other FACT-OF relations.
* Note also, that we do it like this such that C[FACT-OF] can point from RELATION metaclass to RELATION metaclass which is sound.
* If we'd relax this, we could also do:
*
* FACT-OF <-- C[FACT-OF]
*             ^--|  |--^
*
* If we also pull C[FACT-OF] outside of the relation domain (e.g. as a third meta-model besides CONCEPT & RELATION) we could do:
*
* FACT-OF <-- C[FACT-OF]
*
* Currently, the second solution has been chosen, because it nicely fits into the picture of the class pointing to the instances.
*
* Apart from the common relations introduced here, there are convenience functions based on them which will ease the interpretation
* of graphs build from them.
* 
*/

class CommonConceptGraph : public Conceptgraph
{
    public:
        /*As stated above this HEDGE will point FROM all RELATIONS which encode FACT-OF relations. It also points FROM AND TO itself!!!*/
        static const UniqueId FactOfId;
        /*This relation cannot be subclassed by itself!*/
        static const UniqueId SubrelOfId;
        /*Other predefined relations*/
        static const UniqueId IsAId;
        static const UniqueId HasAId;
        static const UniqueId PartOfId;
        static const UniqueId ConnectsId;
        static const UniqueId InstanceOfId;

        /*Constructors*/
        CommonConceptGraph();
        CommonConceptGraph(const Hypergraph& base);
        void createCommonConcepts();
        // TODO: Need a CommonConceptGraph destroy() method. This allows to cleanup leftovers (e.g. dangling facts etc.)

        /*Make facts*/
        Hyperedges factOf(const Hyperedges& factIds, const Hyperedges& superRelIds);
        Hyperedges factFrom(const Hyperedges& fromIds, const Hyperedges& toIds, const UniqueId superId);
        Hyperedges factFrom(const Hyperedges& fromIds, const Hyperedges& toIds, const Hyperedges& superIds);
        Hyperedges factFromAnother(const Hyperedges& fromIds, const Hyperedges& toIds, const Hyperedges& otherIds); // Creates another fact of the same superclass than otherId(s)

        /*Constructive functions using predefined relations*/
        Hyperedges subrelationOf(const Hyperedges& subRelId, const Hyperedges& superRelId);
        Hyperedges subrelationFrom(const UniqueId& subRelId, const Hyperedges& fromIds, const Hyperedges& toIds, const UniqueId& superRelId);
        Hyperedges isA(const Hyperedges& subId, const Hyperedges& superId);
        Hyperedges hasA(const Hyperedges& parentId, const Hyperedges& childId);
        Hyperedges partOf(const Hyperedges& partId, const Hyperedges& wholeId);
        Hyperedges connects(const Hyperedges& connectorId, const Hyperedges& interfaceId);
        Hyperedges instanceOf(const Hyperedges& individualIds, const Hyperedges& superIds);

        /*Constructive functions creating new concepts*/
        Hyperedges createSubclassOf(const UniqueId& subId, const Hyperedges& superIds, const std::string& label=""); // create subId,label <-- IS-A --> superId(s)
        Hyperedges instantiateFrom(const UniqueId superId, const std::string& label=""); // create label <-- INSTANCE-OF --> superId
        Hyperedges instantiateFrom(const Hyperedges& superIds, const std::string& label="");
        Hyperedges instantiateAnother(const Hyperedges& otherIds, const std::string& label=""); // create another instance from the superclasses of others

        /*Common queries*/
        // factsOf will return all facts of superRelId, which points from fromIds (or any) and to toIds (or any), FORWARD direction will return superRels instead and label filters the facts by label
        Hyperedges factsOf(const UniqueId& superRelId, const Hyperedges& fromIds=Hyperedges(), const Hyperedges& toIds=Hyperedges(), const TraversalDirection dir=INVERSE, const std::string& label="") const;
        Hyperedges factsOf(const Hyperedges& superRelIds, const Hyperedges& fromIds=Hyperedges(), const Hyperedges& toIds=Hyperedges(), const TraversalDirection dir=INVERSE, const std::string& label="") const;
        Hyperedges subrelationsOf(const UniqueId superRelId, const std::string& label="", const TraversalDirection dir=INVERSE) const;    //transitive subrelOf
        Hyperedges subrelationsOf(const Hyperedges& superRelIds, const std::string& label="", const TraversalDirection dir=INVERSE) const;    //transitive subrelOf
        Hyperedges directSubrelationsOf(const Hyperedges& superRelIds, const std::string& label="", const TraversalDirection dir=INVERSE) const;    //non-transitive subrelOf

        // TODO: We need traverse(const UniqueId& rootId, const Hyperedges& classUids, const Hyperedges& superRelUids, const TraversalDirection dir)
        // This allows us to traverse by looking to isA/instanceOf, subrelOf/factOf relationships?

        /*Other common queries using subrelationsOf!*/
        /*NOTE: The traversal direction tells if the basic relation is to be followed in its direction(FORWARD) or against it(INVERSE)*/
        Hyperedges transitiveClosure(const UniqueId& rootId, const UniqueId& relId, const std::string& label="", const TraversalDirection dir=FORWARD) const;
        Hyperedges subclassesOf(const Hyperedges& ids, const std::string& label="", const TraversalDirection dir=INVERSE) const;      //transitive isA
        Hyperedges partsOf(const Hyperedges& ids, const std::string& label="", const TraversalDirection dir=INVERSE) const;           //transitive partOf
        Hyperedges descendantsOf(const Hyperedges& ancestorIds, const std::string& label="", const TraversalDirection dir=FORWARD) const; //transitive hasA
        Hyperedges directSubclassesOf(const Hyperedges& ids, const std::string& label="", const TraversalDirection dir=INVERSE) const; // non-transitive isA
        Hyperedges componentsOf(const Hyperedges& ids, const std::string& label="", const TraversalDirection dir=INVERSE) const;     //non-transitive partOf
        Hyperedges instancesOf(const Hyperedges& ids, const std::string& label="", const TraversalDirection dir=INVERSE) const;      //non-transitive instanceOf
        Hyperedges childrenOf(const Hyperedges& ids, const std::string& label="", const TraversalDirection dir=FORWARD) const;     //non-transitive hasA
        Hyperedges endpointsOf(const Hyperedges& ids, const std::string& label="", const TraversalDirection dir=FORWARD) const;    //non-transitive connects

        /*Shortcuts*/
        Hyperedges subrelationOf(const UniqueId subRelId, const UniqueId superRelId)
        {
            return subrelationOf(Hyperedges{subRelId}, Hyperedges{superRelId});
        }
        Hyperedges subclassesOf(const UniqueId id, const std::string& label="", const TraversalDirection dir=INVERSE) const
        {
            return subclassesOf(Hyperedges{id}, label, dir);
        }
        Hyperedges isA(const UniqueId subId, const UniqueId superId)
        {
            return isA(Hyperedges{subId}, Hyperedges{superId});
        }
        Hyperedges isA(const Hyperedges& subIds, const UniqueId superId)
        {
            return isA(subIds, Hyperedges{superId});
        }
        Hyperedges childrenOf(const UniqueId id, const std::string& label="", const TraversalDirection dir=FORWARD) const
        {
            return childrenOf(Hyperedges{id}, label, dir);
        }
        Hyperedges instancesOf(const UniqueId id, const std::string& label="", const TraversalDirection dir=INVERSE) const
        {
            return instancesOf(Hyperedges{id}, label, dir);
        }

        /* Generic mapping algorithm */
        // PartitionFuncLeft should return the hedges to be mapped to the ones returned by
        // PartitionFuncRight. The signature is Hyperedges (CommonConceptGraph&)
        // The matchFunc m should return the potential costs, whenever two concepts shall be matched. The signature is float (CommonConceptGraph&, UniqueId, UniqueId)
        // The mapFunc mp should map the two concepts and update the corresponding resources. The signature is void (CommonConceptGraph&, UniqueId, UniqueId)
        template<typename PartitionFuncLeft, typename PartitionFuncRight,  typename MatchFunc, typename MapFunc > CommonConceptGraph map (PartitionFuncLeft pl, PartitionFuncRight pr, MatchFunc m, MapFunc mp) const;
};

// Include template member functions
// See http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
#include "CommonConceptGraph.tpp"

#endif
