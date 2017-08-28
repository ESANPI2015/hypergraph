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
        Hyperedges factOf(const Hyperedges& factIds, const Hyperedges& superRelIds);
        Hyperedges relateFrom(const Hyperedges& fromIds, const Hyperedges& toIds, const unsigned superId);
        Hyperedges relateFrom(const Hyperedges& fromIds, const Hyperedges& toIds, const Hyperedges& superIds);

        /*Constructive functions using predefined relations*/
        Hyperedges subrelationOf(const Hyperedges& subRelId, const Hyperedges& superRelId);
        Hyperedges isA(const Hyperedges& subId, const Hyperedges& superId);
        Hyperedges hasA(const Hyperedges& parentId, const Hyperedges& childId);
        Hyperedges partOf(const Hyperedges& partId, const Hyperedges& wholeId);
        Hyperedges connects(const Hyperedges& connectorId, const Hyperedges& interfaceId);
        Hyperedges instanceOf(const Hyperedges& individualIds, const Hyperedges& superIds);
        Hyperedges instantiateFrom(const unsigned superId, const std::string& label="");
        Hyperedges instantiateFrom(const Hyperedges& superIds, const std::string& label="");

        /*Common queries*/
        Hyperedges factsOf(const unsigned superRelId, const std::string& label="");           // non-transitive
        Hyperedges factsOf(const Hyperedges& superRelIds, const std::string& label="");       // non-transitive
        Hyperedges subrelationsOf(const unsigned superRelId, const std::string& label="");    //transitive

        /*Other common queries using subrelationsOf!*/
        /*NOTE: The traversal direction tells if the basic relation is to be followed in its direction(DOWN) or against it(UP)*/
        Hyperedges transitiveClosure(const unsigned rootId, const unsigned relId, const std::string& label="", const TraversalDirection dir=DOWN);
        Hyperedges subclassesOf(const Hyperedges& ids, const std::string& label="", const TraversalDirection dir=UP);      //transitive
        Hyperedges partsOf(const Hyperedges& ids, const std::string& label="", const TraversalDirection dir=UP);           //transitive
        Hyperedges instancesOf(const Hyperedges& ids, const std::string& label="", const TraversalDirection dir=UP);       //non-transitive
        Hyperedges childrenOf(const Hyperedges& ids, const std::string& label="", const TraversalDirection dir=DOWN);     //non-transitive

        /*Shortcuts*/
        Hyperedges subrelationOf(const unsigned subRelId, const unsigned superRelId)
        {
            return subrelationOf(Hyperedges{subRelId}, Hyperedges{superRelId});
        }
        Hyperedges subclassesOf(const unsigned id, const std::string& label="", const TraversalDirection dir=UP)
        {
            return subclassesOf(Hyperedges{id}, label, dir);
        }
        Hyperedges isA(const unsigned subId, const unsigned superId)
        {
            return isA(Hyperedges{subId}, Hyperedges{superId});
        }
        Hyperedges isA(const Hyperedges& subIds, const unsigned superId)
        {
            return isA(subIds, Hyperedges{superId});
        }
        Hyperedges childrenOf(const unsigned id, const std::string& label="", const TraversalDirection dir=DOWN)
        {
            return childrenOf(Hyperedges{id}, label, dir);
        }
        Hyperedges instancesOf(const unsigned id, const std::string& label="", const TraversalDirection dir=UP)
        {
            return instancesOf(Hyperedges{id}, label, dir);
        }
};

#endif
