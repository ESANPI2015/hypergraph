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

Hyperedges CommonConceptGraph::factOf(const Hyperedges& factIds, const Hyperedges& superRelIds)
{
    // Is it as simple as this?
    Hyperedges id;
    Hyperedges fromIds = intersect(Conceptgraph::relations(), factIds);
    Hyperedges toIds = intersect(Conceptgraph::relations(), superRelIds);
    if (fromIds.size() && toIds.size())
    {
        id = Conceptgraph::relate(fromIds, toIds, CommonConceptGraph::FactOfId);
        if (!id.empty())
        {
            // If we have created a factOf a factOf superrelation :D , we have to link them
            from(subtract(subtract(id, fromIds), toIds), CommonConceptGraph::FactOfId);
        }
    }
    return id;
}

Hyperedges CommonConceptGraph::relateFrom(const Hyperedges& fromIds, const Hyperedges& toIds, const unsigned superId)
{
    // At first create the relation ...
    Hyperedges id;
    if (fromIds.size() && toIds.size())
    {
        id = Conceptgraph::relate(fromIds, toIds, superId);
        if (!id.empty())
        {
            // ... then make the new relation a factOf the super relation
            factOf(subtract(subtract(id, fromIds), toIds), Hyperedges{superId});
        }
    }
    return id;
}

Hyperedges CommonConceptGraph::relateFrom(const Hyperedges& fromIds, const Hyperedges& toIds, const Hyperedges& superIds)
{
    Hyperedges result;
    for (unsigned superId : superIds)
    {
        result = unite(result, relateFrom(fromIds, toIds, superId));
    }
    return result;
}

Hyperedges CommonConceptGraph::subrelationOf(const Hyperedges& subRelIds, const Hyperedges& superRelIds)
{
    Hyperedges id;
    Hyperedges fromIds = intersect(Conceptgraph::relations(), subRelIds);
    Hyperedges toIds = intersect(Conceptgraph::relations(), superRelIds);
    if (fromIds.size() && toIds.size())
    {
        id = relateFrom(fromIds, toIds, CommonConceptGraph::SubrelOfId);
    }
    return id;
}

Hyperedges CommonConceptGraph::isA(const Hyperedges& subIds, const Hyperedges& superIds)
{
    Hyperedges id;
    Hyperedges fromIds = intersect(Conceptgraph::find(), subIds);
    Hyperedges toIds = intersect(Conceptgraph::find(), superIds);
    if (fromIds.size() && toIds.size())
    {
        id = relateFrom(fromIds, toIds, CommonConceptGraph::IsAId);
    }
    return id;
}

Hyperedges CommonConceptGraph::hasA(const Hyperedges& parentIds, const Hyperedges& childIds)
{
    Hyperedges id;
    Hyperedges fromIds = intersect(Conceptgraph::find(), parentIds);
    Hyperedges toIds = intersect(Conceptgraph::find(), childIds);
    if (fromIds.size() && toIds.size())
    {
        id = relateFrom(fromIds, toIds, CommonConceptGraph::HasAId);
    }
    return id;
}

Hyperedges CommonConceptGraph::partOf(const Hyperedges& partIds, const Hyperedges& wholeIds)
{
    Hyperedges id;
    Hyperedges fromIds = intersect(unite(Conceptgraph::find(),Conceptgraph::relations()), partIds); // To encode nested subgraphs
    Hyperedges toIds = intersect(Conceptgraph::find(), wholeIds);
    if (fromIds.size() && toIds.size())
    {
        id = relateFrom(fromIds, toIds, CommonConceptGraph::PartOfId);
    }
    return id;
}

Hyperedges CommonConceptGraph::connects(const Hyperedges& connectorIds, const Hyperedges& interfaceIds)
{
    Hyperedges id;
    Hyperedges fromIds = intersect(Conceptgraph::find(), connectorIds);
    Hyperedges toIds = intersect(Conceptgraph::find(), interfaceIds);
    if (fromIds.size() && toIds.size())
    {
        id = relateFrom(fromIds, toIds, CommonConceptGraph::ConnectsId);
    }
    return id;
}

Hyperedges CommonConceptGraph::instanceOf(const Hyperedges& individualIds, const Hyperedges& superIds)
{
    return relateFrom(intersect(Conceptgraph::find(), individualIds), intersect(Conceptgraph::find(), superIds), CommonConceptGraph::InstanceOfId);
}

Hyperedges CommonConceptGraph::instantiateFrom(const unsigned superId, const std::string& label)
{
    std::string theLabel = label;
    if (theLabel.empty())
    {
        theLabel = Hypergraph::get(superId)->label();
    }

    Hyperedges id = Conceptgraph::create(theLabel);
    instanceOf(Hyperedges{id}, Hyperedges{superId});
    return id;
}

Hyperedges CommonConceptGraph::instantiateFrom(const Hyperedges& superIds, const std::string& label)
{
    Hyperedges result;
    for (unsigned superId : superIds)
    {
        result = unite(result, instantiateFrom(superId, label));
    }
    return result;
}

Hyperedges CommonConceptGraph::factsOf(const unsigned superRelId, const std::string& label)
{
    Hyperedges result;
    // NOTE: all contains FACT-OF with (FACT-OF --> superRelId)
    Hyperedges all = Conceptgraph::relationsTo(superRelId, get(CommonConceptGraph::FactOfId)->label());
    // ... the result will then contain all facts with (facts <-- FACT-OF --> superRelId)
    result = Hypergraph::from(all, label);
    return result;
}

Hyperedges CommonConceptGraph::factsOf(const Hyperedges& superRelIds, const std::string& label)
{
    Hyperedges result;
    for (unsigned superRelId : superRelIds)
    {
        Hyperedges some = factsOf(superRelId, label);
        result.insert(some.begin(), some.end());
    }
    return result;
}

Hyperedges CommonConceptGraph::subrelationsOf(const unsigned superRelId, const std::string& label)
{
    // Here we start a traversal from superRelId following every subrelationOf relation
    // Since this cannot be subclassed/relabelled, we can use a simple traverse function
    return Conceptgraph::traverse(superRelId, label, get(CommonConceptGraph::SubrelOfId)->label(), UP);
}

Hyperedges CommonConceptGraph::transitiveClosure(const unsigned rootId, const unsigned relId, const std::string& label, const TraversalDirection dir)
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

Hyperedges CommonConceptGraph::subclassesOf(const Hyperedges& superIds, const std::string& label, const TraversalDirection dir)
{
    Hyperedges result;
    for (unsigned id : superIds)
    {
        result = unite(result, transitiveClosure(id, CommonConceptGraph::IsAId, label, dir));
    }
    return result;
}

Hyperedges CommonConceptGraph::partsOf(const Hyperedges& wholeIds, const std::string& label, const TraversalDirection dir)
{
    Hyperedges result;
    for (unsigned id : wholeIds)
    {
        result = unite(result, transitiveClosure(id, CommonConceptGraph::PartOfId, label, dir));
    }
    return result;
}

Hyperedges CommonConceptGraph::instancesOf(const Hyperedges& ids, const std::string& label, const TraversalDirection dir)
{
    Hyperedges result;
    // Get all subrelationsOf INSTANCE-OF
    Hyperedges subRels = subrelationsOf(CommonConceptGraph::InstanceOfId);
    // Get all factsOf these subrelations
    Hyperedges facts = factsOf(subRels);

    switch (dir)
    {
        case DOWN:
        {
            // Get all relationsFrom id
            Hyperedges relsFromIndividuals = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfInstanceOf --> X) relations
            Hyperedges relevantRels = intersect(relsFromIndividuals, facts);
            // Get all these X matching the label
            result = Hypergraph::to(relevantRels, label);
        }
        break;
        case BOTH:
        {
            // Get all relationsFrom id
            Hyperedges relsFromIndividuals = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfInstanceOf --> X) relations
            Hyperedges relevantRels = intersect(relsFromIndividuals, facts);
            // Get all these X matching the label
            result = Hypergraph::to(relevantRels, label);
        }
        case UP:
        {
            // Get all relationsTo id
            Hyperedges relsToSupers = Conceptgraph::relationsTo(ids);
            // Contains all (X <-- factFromSubRelOfInstanceOf --> id) relations
            Hyperedges relevantRels = intersect(relsToSupers, facts);
            // Get all these X matching the label
            result = Hypergraph::from(relevantRels, label);
        }
        break;
    }
    return result;
}

Hyperedges CommonConceptGraph::childrenOf(const Hyperedges& ids, const std::string& label, const TraversalDirection dir)
{
    Hyperedges result;
    // Get all subrelationsOf HAS-A
    Hyperedges subRels = subrelationsOf(CommonConceptGraph::HasAId);
    // Get all factsOf these subrelations
    Hyperedges facts = factsOf(subRels);

    switch (dir)
    {
        case DOWN:
        {
            // Get all relationsFrom ids
            Hyperedges relsFromParents = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfHasA --> X) relations
            Hyperedges relevantRels = intersect(relsFromParents, facts);
            // Get all these X matching the label
            result = Hypergraph::to(relevantRels, label);
        }
        break;
        case BOTH:
        {
            // Get all relationsFrom ids
            Hyperedges relsFromParents = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfHasA --> X) relations
            Hyperedges relevantRels = intersect(relsFromParents, facts);
            // Get all these X matching the label
            result = Hypergraph::to(relevantRels, label);
        }
        case UP:
        {
            // Get all relationsTo id
            Hyperedges relsToParents = Conceptgraph::relationsTo(ids);
            // Contains all (X <-- factFromSubRelOfHasA --> id) relations
            Hyperedges relevantRels = intersect(relsToParents, facts);
            // Get all these X matching the label
            result = Hypergraph::from(relevantRels, label);
        }
        break;
    }
    return result;
}

Hyperedges CommonConceptGraph::endpointsOf(const Hyperedges& ids, const std::string& label, const TraversalDirection dir)
{
    Hyperedges result;
    // Get all subrelationsOf CONNECTS
    Hyperedges subRels = subrelationsOf(CommonConceptGraph::ConnectsId);
    // Get all factsOf these subrelations
    Hyperedges facts = factsOf(subRels);

    switch (dir)
    {
        case DOWN:
        {
            // Get all relationsFrom ids
            Hyperedges relsFromConnectors = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfHasA --> X) relations
            Hyperedges relevantRels = intersect(relsFromConnectors, facts);
            // Get all these X matching the label
            result = Hypergraph::to(relevantRels, label);
        }
        break;
        case BOTH:
        {
            // Get all relationsFrom ids
            Hyperedges relsFromConnectors = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfHasA --> X) relations
            Hyperedges relevantRels = intersect(relsFromConnectors, facts);
            // Get all these X matching the label
            result = Hypergraph::to(relevantRels, label);
        }
        case UP:
        {
            // Get all relationsTo id
            Hyperedges relsToConnectors = Conceptgraph::relationsTo(ids);
            // Contains all (X <-- factFromSubRelOfHasA --> id) relations
            Hyperedges relevantRels = intersect(relsToConnectors, facts);
            // Get all these X matching the label
            result = Hypergraph::from(relevantRels, label);
        }
        break;
    }
    return result;
}
