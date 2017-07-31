#include "CommonConceptGraph.hpp"
//#include <iostream>

const unsigned CommonConceptGraph::FactOfId = 3;
const unsigned CommonConceptGraph::SubrelOfId = 4;
const unsigned CommonConceptGraph::IsAId = 5;
const unsigned CommonConceptGraph::HasAId = 6;
const unsigned CommonConceptGraph::PartOfId = 7;
const unsigned CommonConceptGraph::ConnectsId = 8;
const unsigned CommonConceptGraph::InstanceOfId = 9;

void CommonConceptGraph::createCommonConcepts()
{
    /* Common relations over relations*/
    Conceptgraph::relate(CommonConceptGraph::FactOfId, Conceptgraph::RelationId, Conceptgraph::RelationId, "FACT-OF");
    Conceptgraph::relate(CommonConceptGraph::SubrelOfId, Conceptgraph::RelationId, Conceptgraph::RelationId, "SUBREL-OF");

    /* Common relations over concepts*/
    Conceptgraph::relate(CommonConceptGraph::IsAId, Conceptgraph::ConceptId, Conceptgraph::ConceptId, "IS-A");
    Conceptgraph::relate(CommonConceptGraph::HasAId, Conceptgraph::ConceptId, Conceptgraph::ConceptId, "HAS-A");
    Conceptgraph::relate(CommonConceptGraph::PartOfId, Conceptgraph::ConceptId, Conceptgraph::ConceptId, "PART-OF");
    Conceptgraph::relate(CommonConceptGraph::ConnectsId, Conceptgraph::ConceptId, Conceptgraph::ConceptId, "CONNECTS");
    Conceptgraph::relate(CommonConceptGraph::InstanceOfId, Conceptgraph::ConceptId, Conceptgraph::ConceptId, "INSTANCE-OF");

    // Make factOf point from and to itself
    get(CommonConceptGraph::FactOfId)->from(CommonConceptGraph::FactOfId);
    get(CommonConceptGraph::FactOfId)->to(CommonConceptGraph::FactOfId);
}

CommonConceptGraph::CommonConceptGraph()
: Conceptgraph()
{
    createCommonConcepts();
}

CommonConceptGraph::CommonConceptGraph(Conceptgraph& A)
: Conceptgraph(A)
{
    createCommonConcepts();
}

unsigned CommonConceptGraph::factOf(const unsigned factId, const unsigned superRelId)
{
    // Is it as simple as this?
    // TODO:
    // * Currently, relate can fail giving id=0 ... in this case that means that there already exists a factOf relation between factId and superRelId?
    unsigned id = 0;
    if (Conceptgraph::relations().count(factId) && Conceptgraph::relations().count(superRelId))
    {
        id = Conceptgraph::relate(factId, superRelId, CommonConceptGraph::FactOfId);
        if (id)
        {
            // If we have created a factOf a factOf superrelation :D , we have to link them
            get(CommonConceptGraph::FactOfId)->from(id);
        }
    }
    return id;
}

unsigned CommonConceptGraph::relateFrom(const unsigned fromId, const unsigned toId, const unsigned superId)
{
    // At first create the relation ...
    unsigned id = Conceptgraph::relate(fromId, toId, superId);
    if (id)
    {
        // ... then make the new relation a factOf the super relation
        factOf(id, superId);
    }
    return id;
}

unsigned CommonConceptGraph::relateFrom(const Hyperedges& fromIds, const Hyperedges& toIds, const unsigned superId)
{
    // At first create the relation ...
    unsigned id = Conceptgraph::relate(fromIds, toIds, superId);
    if (id)
    {
        // ... then make the new relation a factOf the super relation
        factOf(id, superId);
    }
    return id;
}

unsigned CommonConceptGraph::subrelationOf(const unsigned subRelId, const unsigned superRelId)
{
    unsigned id = 0;
    if (Conceptgraph::relations().count(subRelId) && Conceptgraph::relations().count(superRelId))
    {
        id = relateFrom(subRelId, superRelId, CommonConceptGraph::SubrelOfId);
    }
    return id;
}

unsigned CommonConceptGraph::isA(const unsigned subId, const unsigned superId)
{
    unsigned id = 0;
    if (Conceptgraph::find().count(subId) && Conceptgraph::find().count(superId))
    {
        id = relateFrom(subId, superId, CommonConceptGraph::IsAId);
    }
    return id;
}

unsigned CommonConceptGraph::hasA(const unsigned parentId, const unsigned childId)
{
    unsigned id = 0;
    if (Conceptgraph::find().count(parentId) && Conceptgraph::find().count(childId))
    {
        id = relateFrom(parentId, childId, CommonConceptGraph::HasAId);
    }
    return id;
}
unsigned CommonConceptGraph::partOf(const unsigned partId, const unsigned wholeId)
{
    unsigned id = 0;
    if (Conceptgraph::find().count(partId) && Conceptgraph::find().count(wholeId))
    {
        id = relateFrom(partId, wholeId, CommonConceptGraph::PartOfId);
    }
    return id;
}
unsigned CommonConceptGraph::connects(const unsigned connectorId, const unsigned interfaceId)
{
    unsigned id = 0;
    if (Conceptgraph::find().count(connectorId) && Conceptgraph::find().count(interfaceId))
    {
        id = relateFrom(connectorId, interfaceId, CommonConceptGraph::ConnectsId);
    }
    return id;
}

unsigned CommonConceptGraph::instanceOf(const unsigned individualId, const unsigned superId)
{
    unsigned id = 0;
    if (Conceptgraph::find().count(individualId) && Conceptgraph::find().count(superId))
    {
        id = relateFrom(individualId, superId, CommonConceptGraph::InstanceOfId);
    }
    return id;
}

unsigned CommonConceptGraph::instanceOf(const Hyperedges& individualIds, const Hyperedges& superIds)
{
    return relateFrom(intersect(Conceptgraph::find(), individualIds), intersect(Conceptgraph::find(), superIds), CommonConceptGraph::InstanceOfId);
}

Hypergraph::Hyperedges CommonConceptGraph::factsOf(const unsigned superRelId, const std::string& label)
{
    Hyperedges result;
    // NOTE: all contains FACT-OF with (FACT-OF --> superRelId)
    Hyperedges all = Conceptgraph::relationsTo(superRelId, get(CommonConceptGraph::FactOfId)->label());
    // ... the result will then contain all facts with (facts <-- FACT-OF --> superRelId)
    result = Hypergraph::from(all, label);
    return result;
}

Hypergraph::Hyperedges CommonConceptGraph::factsOf(const Hyperedges& superRelIds, const std::string& label)
{
    Hyperedges result;
    for (unsigned superRelId : superRelIds)
    {
        Hyperedges some = factsOf(superRelId, label);
        result.insert(some.begin(), some.end());
    }
    return result;
}

Hypergraph::Hyperedges CommonConceptGraph::subrelationsOf(const unsigned superRelId, const std::string& label)
{
    // Here we start a traversal from superRelId following every subrelationOf relation
    // Since this cannot be subclassed/relabelled, we can use a simple traverse function
    return Conceptgraph::traverse(superRelId, label, get(CommonConceptGraph::SubrelOfId)->label(), UP);
}

Hypergraph::Hyperedges CommonConceptGraph::transitiveClosure(const unsigned rootId, const unsigned relId, const std::string& label, const TraversalDirection dir)
{
    // At first, find all relations we have to consider during traversal:
    // These are all subrelations of relId including relId itself
    Hyperedges relationsToFollow = subrelationsOf(relId);
    std::vector<std::string> relLabels;
    for (unsigned id : relationsToFollow)
    {
        if (!get(id)->label().empty())
            relLabels.push_back(get(id)->label());
    }

    // Then we have to traverse the hypergraph starting at rootId,
    // checking label of visited hedges,
    // and following every relation which has a label matching one of the relations to be followed
    std::vector<std::string> visitedLabels;
    if (!label.empty())
        visitedLabels.push_back(label);
    return Conceptgraph::traverse(rootId, visitedLabels, relLabels, dir);
}

Hypergraph::Hyperedges CommonConceptGraph::subclassesOf(const unsigned superId, const std::string& label)
{
    return transitiveClosure(superId, CommonConceptGraph::IsAId, label, UP);
}

Hypergraph::Hyperedges CommonConceptGraph::partsOf(const unsigned wholeId, const std::string& label)
{
    return transitiveClosure(wholeId, CommonConceptGraph::PartOfId, label, UP);
}

Hypergraph::Hyperedges CommonConceptGraph::instancesOf(const unsigned superId, const std::string& label)
{
    Hyperedges result;
    // Get all subrelationsOf INSTANCE-OF
    Hyperedges subRels = subrelationsOf(CommonConceptGraph::InstanceOfId);
    // Get all factsOf these subrelations
    Hyperedges facts = factsOf(subRels);
    // Get all relationsTo superId
    Hyperedges relsToSuper = Conceptgraph::relationsTo(superId);

    // Contains all (X <-- factFromSubRelOfInstanceOf --> superId) relations
    Hyperedges relevantRels = intersect(relsToSuper, facts);
    // Get all these X matching the label
    result = Hypergraph::from(relevantRels, label);
    return result;
}

Hypergraph::Hyperedges CommonConceptGraph::childrenOf(const unsigned parentId, const std::string& label)
{
    Hyperedges result;
    // Get all subrelationsOf HAS-A
    Hyperedges subRels = subrelationsOf(CommonConceptGraph::HasAId);
    // Get all factsOf these subrelations
    Hyperedges facts = factsOf(subRels);
    // Get all relationsFrom parentId
    Hyperedges relsFromParent = Conceptgraph::relationsFrom(parentId);

    // Contains all (parentId <-- factFromSubRelOfHasA --> X) relations
    Hyperedges relevantRels = intersect(relsFromParent, facts);
    // Get all these X mathcing the label
    result = Hypergraph::to(relevantRels, label);
    return result;
}
