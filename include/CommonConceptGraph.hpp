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
        static const unsigned FactOfId;
        /*This relation cannot be subclassed by itself!*/
        static const unsigned SubrelOfId;

        /*Other predefined relations*/
        static const unsigned IsAId;
        static const unsigned HasAId;
        static const unsigned PartOfId;
        static const unsigned ConnectsId;
        static const unsigned InstanceOfId;

        void createCommonConcepts();

        /*Constructors*/
        CommonConceptGraph();
        CommonConceptGraph(Conceptgraph& base);

        /*Make facts*/
        unsigned factOf(const unsigned factId, const unsigned superRelId);
        unsigned relateFrom(const unsigned fromId, const unsigned toId, const unsigned superId);
        unsigned relateFrom(const Hyperedges& fromIds, const Hyperedges& toIds, const unsigned superId);

        /*Constructive functions using predefined relations*/
        unsigned subrelationOf(const unsigned subRelId, const unsigned superRelId);
        unsigned isA(const unsigned subId, const unsigned superId);
        unsigned hasA(const unsigned parentId, const unsigned childId);
        unsigned partOf(const unsigned partId, const unsigned wholeId);
        unsigned connects(const unsigned connectorId, const unsigned interfaceId);
        unsigned instanceOf(const unsigned individualId, const unsigned superId);
        unsigned instanceOf(const Hyperedges& individualIds, const Hyperedges& superIds);
        unsigned instantiateFrom(const unsigned superId, const std::string& label="");

        /*Common queries*/
        Hyperedges factsOf(const unsigned superRelId, const std::string& label="");        //non-transitive
        Hyperedges factsOf(const Hyperedges& superRelIds, const std::string& label="");
        Hyperedges subrelationsOf(const unsigned superRelId, const std::string& label=""); //transitive

        /*Other common queries using subrelationsOf!*/
        Hyperedges transitiveClosure(const unsigned rootId, const unsigned relId, const std::string& label="", const TraversalDirection dir=DOWN);
        Hyperedges subclassesOf(const unsigned superId, const std::string& label="");      //transitive
        Hyperedges superclassesOf(const unsigned subId, const std::string& label="");      //transitive
        Hyperedges partsOf(const unsigned wholeId, const std::string& label="");           //transitive
        Hyperedges instancesOf(const unsigned superId, const std::string& label="");       //non-transitive
        Hyperedges classesOf(const unsigned individualId, const std::string& label="");       //non-transitive
        Hyperedges childrenOf(const unsigned parentId, const std::string& label="");       //non-transitive
};

#endif
