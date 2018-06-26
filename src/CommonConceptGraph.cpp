#include "CommonConceptGraph.hpp"
#include <iostream>

const UniqueId CommonConceptGraph::FactOfId = "CommonConceptGraph::FactOf";
const UniqueId CommonConceptGraph::SubrelOfId = "CommonConceptGraph::SubrelOf";
const UniqueId CommonConceptGraph::IsAId = "CommonConceptGraph::IsA";
const UniqueId CommonConceptGraph::HasAId = "CommonConceptGraph::HasA";
const UniqueId CommonConceptGraph::PartOfId = "CommonConceptGraph::PartOf";
const UniqueId CommonConceptGraph::ConnectsId = "CommonConceptGraph::Connects";
const UniqueId CommonConceptGraph::InstanceOfId = "CommonConceptGraph::InstanceOf";

void CommonConceptGraph::createCommonConcepts()
{
    /* Common relations over relations*/
    Conceptgraph::relate(CommonConceptGraph::FactOfId, Hyperedges{Conceptgraph::IsRelationId}, Hyperedges{Conceptgraph::IsRelationId}, "FACT-OF");
    Conceptgraph::relate(CommonConceptGraph::SubrelOfId, Hyperedges{Conceptgraph::IsRelationId}, Hyperedges{Conceptgraph::IsRelationId}, "SUBREL-OF");

    /* Common relations over concepts*/
    Conceptgraph::relate(CommonConceptGraph::IsAId, Hyperedges{Conceptgraph::IsConceptId}, Hyperedges{Conceptgraph::IsConceptId}, "IS-A");
    Conceptgraph::relate(CommonConceptGraph::HasAId, Hyperedges{Conceptgraph::IsConceptId}, Hyperedges{Conceptgraph::IsConceptId}, "HAS-A");
    Conceptgraph::relate(CommonConceptGraph::PartOfId, Hyperedges{Conceptgraph::IsConceptId}, Hyperedges{Conceptgraph::IsConceptId}, "PART-OF");
    Conceptgraph::relate(CommonConceptGraph::ConnectsId, Hyperedges{Conceptgraph::IsConceptId}, Hyperedges{Conceptgraph::IsConceptId}, "CONNECTS");
    Conceptgraph::relate(CommonConceptGraph::InstanceOfId, Hyperedges{Conceptgraph::IsConceptId}, Hyperedges{Conceptgraph::IsConceptId}, "INSTANCE-OF");

    // Make factOf point from and to itself
    get(CommonConceptGraph::FactOfId)->from(CommonConceptGraph::FactOfId);
    get(CommonConceptGraph::FactOfId)->to(CommonConceptGraph::FactOfId);
}

CommonConceptGraph::CommonConceptGraph()
: Conceptgraph()
{
    createCommonConcepts();
}

CommonConceptGraph::CommonConceptGraph(const Hypergraph& A)
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
        id = Conceptgraph::relateFrom(fromIds, toIds, CommonConceptGraph::FactOfId);
        if (!id.empty())
        {
            // If we have created a factOf a factOf superrelation :D , we have to link them
            Hypergraph::from(subtract(subtract(id, fromIds), toIds), Hyperedges{CommonConceptGraph::FactOfId});
        }
    }
    return id;
}

Hyperedges CommonConceptGraph::factFrom(const Hyperedges& fromIds, const Hyperedges& toIds, const UniqueId superId)
{
    // At first create the relation ...
    Hyperedges id;
    // TODO: What about UNARY relations?
    if (fromIds.size() && toIds.size())
    {
        id = Conceptgraph::relateFrom(fromIds, toIds, superId);
        if (!id.empty())
        {
            // ... then make the new relation a factOf the super relation
            factOf(subtract(subtract(id, fromIds), toIds), Hyperedges{superId});
        }
    }
    return id;
}

Hyperedges CommonConceptGraph::factFrom(const Hyperedges& fromIds, const Hyperedges& toIds, const Hyperedges& superIds)
{
    Hyperedges result;
    for (UniqueId superId : superIds)
    {
        result = unite(result, factFrom(fromIds, toIds, superId));
    }
    return result;
}

Hyperedges CommonConceptGraph::factFromAnother(const Hyperedges& fromIds, const Hyperedges& toIds, const Hyperedges& otherIds)
{
    Hyperedges result;
    for (UniqueId otherId : otherIds)
    {
        // Get the superRels
        Hyperedges superRels = factsOf(otherId, "", TraversalDirection::FORWARD);
        // Create new facts from these superRels
        result = unite(result, factFrom(fromIds, toIds, superRels));
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
        id = factFrom(fromIds, toIds, CommonConceptGraph::SubrelOfId);
    }
    return id;
}

Hyperedges CommonConceptGraph::subrelationFrom(const UniqueId& subRelId, const Hyperedges& fromIds, const Hyperedges& toIds, const UniqueId& superRelId)
{
    Hyperedges id;
    // TODO: What about UNARY relations?
    if (fromIds.size() && toIds.size())
    {
        id = Conceptgraph::relateFrom(subRelId, fromIds, toIds, superRelId);
        if (!id.empty())
        {
            // ... then make the new relation a subrelation of the superrelation
            subrelationOf(subtract(subtract(id, fromIds), toIds), Hyperedges{superRelId});
        }
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
        id = factFrom(fromIds, toIds, CommonConceptGraph::IsAId);
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
        id = factFrom(fromIds, toIds, CommonConceptGraph::HasAId);
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
        id = factFrom(fromIds, toIds, CommonConceptGraph::PartOfId);
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
        id = factFrom(fromIds, toIds, CommonConceptGraph::ConnectsId);
    }
    return id;
}

Hyperedges CommonConceptGraph::instanceOf(const Hyperedges& individualIds, const Hyperedges& superIds)
{
    return factFrom(intersect(Conceptgraph::find(), individualIds), intersect(Conceptgraph::find(), superIds), CommonConceptGraph::InstanceOfId);
}

Hyperedges CommonConceptGraph::createSubclassOf(const UniqueId& subId, const Hyperedges& superIds, const std::string& label)
{
    Hyperedges result(Conceptgraph::create(subId, label));
    isA(result, superIds);
    return result;
}

Hyperedges CommonConceptGraph::instantiateFrom(const UniqueId superId, const std::string& label)
{
    std::string theLabel = label;
    if (theLabel.empty())
    {
        theLabel = Hypergraph::get(superId)->label();
    }

    UniqueId id(superId);
    unsigned i = 1;
    while (Conceptgraph::create(id, theLabel).empty())
    {
        id = superId + std::to_string(i);
        i++;
    }
    instanceOf(Hyperedges{id}, Hyperedges{superId});
    return Hyperedges{id};
}

Hyperedges CommonConceptGraph::instantiateFrom(const Hyperedges& superIds, const std::string& label)
{
    Hyperedges result;
    for (UniqueId superId : superIds)
    {
        result = unite(result, instantiateFrom(superId, label));
    }
    return result;
}

Hyperedges CommonConceptGraph::instantiateAnother(const Hyperedges& otherIds, const std::string& label)
{
    Hyperedges result;
    for (UniqueId otherId : otherIds)
    {
        // Get superclass of other
        Hyperedges superclasses = instancesOf(otherId,"",TraversalDirection::FORWARD);
        // Instantiate from superclass of other
        result = unite(result, instantiateFrom(superclasses, label));
    }
    return result;
}

Hyperedges CommonConceptGraph::instantiateDeepFrom(const Hyperedges& superIds, const std::string& label)
{
    Hyperedges result;
    // Deep instantiation:
    // This means, that we have to get the following for every x
    for (UniqueId superId : superIds)
    {
        std::map< UniqueId, Hyperedges > original2new;
        // Instantiate the new individual
        original2new[superId] = instantiateFrom(superId, label);
        result = unite(result, original2new[superId]);
        // Get all superclasses as well!
        Hyperedges superclassIds = subclassesOf(superId, "", FORWARD);
        // For each of them
        for (UniqueId superDuperId : superclassIds)
        {
            // Get their children
            Hyperedges superChildren = childrenOf(superDuperId);
            Hyperedges relsFromParent = relationsFrom(Hyperedges{superDuperId});
            for (UniqueId originalId : superChildren)
            {
                // And instantiate them
                original2new[originalId] = instantiateAnother(Hyperedges{originalId}, get(originalId)->label());
                // Clone also all relations R which point from superDuperId to originalId
                Hyperedges relsTo = relationsTo(Hyperedges{originalId});
                Hyperedges commonRels = intersect(relsFromParent, relsTo);
                for (UniqueId commonRelId : commonRels)
                {
                    // Create new facts from these common relations
                    original2new[commonRelId] = factFromAnother(original2new[superId], original2new[originalId], Hyperedges{commonRelId});
                }
            }
        }
    }
    return result;
}

Hyperedges CommonConceptGraph::instantiateSuperDeepFrom(const Hyperedges& superIds, const std::string& label)
{
    Hyperedges result;
    // Super Deep instantiation:
    // This means, that we have to get the following for every x
    for (UniqueId superId : superIds)
    {
        // FIXME: WE CAN DO BETTER!!!! ACTUALLY WE CAN JUST MAKE A TRAVERSE BY JOINING THE LABELS OF PARTS-OF AND DESCENDANTS-OF !!!
        // I <- (PD)+ U (DP)+
        // This set I contains all parts of x, their descendants and so forth
        // It also contains the opposite search of the descendants of x, their parts and so forth
        Hyperedges subgraph;
        std::map< UniqueId, Hyperedges > original2new;
        // TODO: Actually, we have to do, what instantiateDeepFrom does as well ... incorporate all superclasses into the search as well!!!
        {
            // TODO: Check if the following actually performs BOTH SEARCHES at once by allowing superId to be in parts!!!
            bool searchParts = false;
            Hyperedges parts, descendants;
            parts = partsOf(Hyperedges{superId});
            subgraph = unite(subgraph, parts);
            do {
                if (searchParts)
                {
                    parts = subtract(partsOf(descendants), descendants);
                    subgraph = unite(subgraph, parts);
                    searchParts = false;
                } else {
                    descendants = subtract(descendantsOf(parts), parts);
                    subgraph = unite(subgraph, descendants);
                    searchParts = true;
                }
            } while (parts.size() || descendants.size());
        }
        //std::cout << "Subgraph: " << subgraph << "\n";
        // Instantiate from superId
        original2new[superId] = instantiateFrom(superId, label);
        result = unite(result, original2new[superId]);
        // All i in I have to be instantiated from their superclasses resulting in a mapping from I to some O
        for (UniqueId originalId : subgraph)
        {
            // Skip superId
            if (originalId == superId)
                continue;
            original2new[originalId] = instantiateAnother(Hyperedges{originalId}, get(originalId)->label());
        }
        // Finally, for each (i1,i2) in I: If i1 related-by-R-to i2, then o1 related-by-R'-to o2
        for (UniqueId originalId : subgraph)
        {
            Hyperedges relsFrom = relationsFrom(Hyperedges{originalId}); // originalId <- X
            for (UniqueId otherOriginalId : subgraph)
            {
                Hyperedges relsTo = relationsTo(Hyperedges{otherOriginalId}); // Y -> otherOriginalId
                Hyperedges commonRels = intersect(relsFrom, relsTo);
                for (UniqueId commonRelId : commonRels)
                {
                    // Create new facts from these superRels
                    original2new[commonRelId] = factFromAnother(original2new[originalId], original2new[otherOriginalId], Hyperedges{commonRelId});
                }
            }
        }
    }
    return result;
}

Hyperedges CommonConceptGraph::factsOf(const UniqueId superRelId, const std::string& label, const TraversalDirection dir)
{
    Hyperedges result;
    switch (dir)
    {
        case INVERSE:
            {
                // NOTE: all contains FACT-OF with (FACT-OF --> superRelId)
                Hyperedges all = Conceptgraph::relationsTo(Hyperedges{superRelId}, get(CommonConceptGraph::FactOfId)->label());
                // ... the result will then contain all facts with (facts <-- FACT-OF --> superRelId)
                result = unite(result, Hypergraph::from(all, label));
            }
            break;
        case BOTH:
            {
                // NOTE: all contains FACT-OF with (FACT-OF --> superRelId)
                Hyperedges all = Conceptgraph::relationsTo(Hyperedges{superRelId}, get(CommonConceptGraph::FactOfId)->label());
                // ... the result will then contain all facts with (facts <-- FACT-OF --> superRelId)
                result = unite(result, Hypergraph::from(all, label));
            }
        case FORWARD:
            {
                // Opposite directions
                Hyperedges all = Conceptgraph::relationsFrom(Hyperedges{superRelId}, get(CommonConceptGraph::FactOfId)->label());
                result = unite(result, Hypergraph::to(all, label));
            }
            break;
    }
    return result;
}

Hyperedges CommonConceptGraph::factsOf(const Hyperedges& superRelIds, const std::string& label, const TraversalDirection dir)
{
    Hyperedges result;
    for (UniqueId superRelId : superRelIds)
    {
        Hyperedges some = factsOf(superRelId, label, dir);
        result.insert(some.begin(), some.end());
    }
    return result;
}

Hyperedges CommonConceptGraph::subrelationsOf(const Hyperedges& superRelIds, const std::string& label, const TraversalDirection dir)
{
    Hyperedges result;
    for (UniqueId superRelId : superRelIds)
        result = unite(result, subrelationsOf(superRelId, label, dir));
    return result;
}
Hyperedges CommonConceptGraph::subrelationsOf(const UniqueId superRelId, const std::string& label, const TraversalDirection dir)
{
    // Here we start a traversal from superRelId following every subrelationOf relation
    // Since this cannot be subclassed/relabelled, we can use a simple traverse function
    return Conceptgraph::traverse(superRelId, label, get(CommonConceptGraph::SubrelOfId)->label(), dir);
}

Hyperedges CommonConceptGraph::transitiveClosure(const UniqueId rootId, const UniqueId relId, const std::string& label, const TraversalDirection dir)
{
    // At first, find all relations we have to consider during traversal:
    // These are all subrelations of relId including relId itself
    Hyperedges relationsToFollow = subrelationsOf(relId);
    std::vector<std::string> relLabels;
    for (UniqueId id : relationsToFollow)
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
    for (UniqueId id : superIds)
    {
        result = unite(result, transitiveClosure(id, CommonConceptGraph::IsAId, label, dir));
    }
    return result;
}

Hyperedges CommonConceptGraph::partsOf(const Hyperedges& wholeIds, const std::string& label, const TraversalDirection dir)
{
    Hyperedges result;
    for (UniqueId id : wholeIds)
    {
        result = unite(result, transitiveClosure(id, CommonConceptGraph::PartOfId, label, dir));
    }
    return result;
}

Hyperedges CommonConceptGraph::descendantsOf(const Hyperedges& ancestorIds, const std::string& label, const TraversalDirection dir)
{
    Hyperedges result;
    for (UniqueId id : ancestorIds)
    {
        result = unite(result, transitiveClosure(id, CommonConceptGraph::HasAId, label, dir));
    }
    return result;
}

Hyperedges CommonConceptGraph::directSubrelationsOf(const Hyperedges& ids, const std::string& label, const TraversalDirection dir)
{
    Hyperedges result;
    // Get all factsOf these SUBREL-OF
    Hyperedges facts = factsOf(CommonConceptGraph::SubrelOfId);

    switch (dir)
    {
        case FORWARD:
        {
            // Get all relationsFrom id
            Hyperedges relsFromSubs = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOf --> X) relations
            Hyperedges relevantRels = intersect(relsFromSubs, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::to(relevantRels, label));
        }
        break;
        case BOTH:
        {
            // Get all relationsFrom id
            Hyperedges relsFromSubs = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOf --> X) relations
            Hyperedges relevantRels = intersect(relsFromSubs, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::to(relevantRels, label));
        }
        case INVERSE:
        {
            // Get all relationsTo id
            Hyperedges relsToSupers = Conceptgraph::relationsTo(ids);
            // Contains all (X <-- factFromSubRelOf --> id) relations
            Hyperedges relevantRels = intersect(relsToSupers, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::from(relevantRels, label));
        }
        break;
    }
    return result;
}

Hyperedges CommonConceptGraph::directSubclassesOf(const Hyperedges& ids, const std::string& label, const TraversalDirection dir)
{
    Hyperedges result;
    // Get all subrelationsOf IS-A
    Hyperedges subRels = subrelationsOf(CommonConceptGraph::IsAId);
    // Get all factsOf these subrelations
    Hyperedges facts = factsOf(subRels);

    switch (dir)
    {
        case FORWARD:
        {
            // Get all relationsFrom id
            Hyperedges relsFromSubs = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfIsA --> X) relations
            Hyperedges relevantRels = intersect(relsFromSubs, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::to(relevantRels, label));
        }
        break;
        case BOTH:
        {
            // Get all relationsFrom id
            Hyperedges relsFromSubs = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfIsA --> X) relations
            Hyperedges relevantRels = intersect(relsFromSubs, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::to(relevantRels, label));
        }
        case INVERSE:
        {
            // Get all relationsTo id
            Hyperedges relsToSupers = Conceptgraph::relationsTo(ids);
            // Contains all (X <-- factFromSubRelOfIsA --> id) relations
            Hyperedges relevantRels = intersect(relsToSupers, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::from(relevantRels, label));
        }
        break;
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
        case FORWARD:
        {
            // Get all relationsFrom id
            Hyperedges relsFromIndividuals = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfInstanceOf --> X) relations
            Hyperedges relevantRels = intersect(relsFromIndividuals, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::to(relevantRels, label));
        }
        break;
        case BOTH:
        {
            // Get all relationsFrom id
            Hyperedges relsFromIndividuals = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfInstanceOf --> X) relations
            Hyperedges relevantRels = intersect(relsFromIndividuals, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::to(relevantRels, label));
        }
        case INVERSE:
        {
            // Get all relationsTo id
            Hyperedges relsToSupers = Conceptgraph::relationsTo(ids);
            // Contains all (X <-- factFromSubRelOfInstanceOf --> id) relations
            Hyperedges relevantRels = intersect(relsToSupers, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::from(relevantRels, label));
        }
        break;
    }
    return result;
}

Hyperedges CommonConceptGraph::componentsOf(const Hyperedges& ids, const std::string& label, const TraversalDirection dir)
{
    Hyperedges result;
    // Get all subrelationsOf partOf
    Hyperedges subRels = subrelationsOf(CommonConceptGraph::PartOfId);
    // Get all factsOf these subrelations
    Hyperedges facts = factsOf(subRels);

    switch (dir)
    {
        case FORWARD:
        {
            // Get all relationsFrom id
            Hyperedges relsFromParts = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfPartOf --> X) relations
            Hyperedges relevantRels = intersect(relsFromParts, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::to(relevantRels, label));
        }
        break;
        case BOTH:
        {
            // Get all relationsFrom id
            Hyperedges relsFromParts = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfPartOf --> X) relations
            Hyperedges relevantRels = intersect(relsFromParts, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::to(relevantRels, label));
        }
        case INVERSE:
        {
            // Get all relationsTo id
            Hyperedges relsToWholes = Conceptgraph::relationsTo(ids);
            // Contains all (X <-- factFromSubRelOfPartOf --> id) relations
            Hyperedges relevantRels = intersect(relsToWholes, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::from(relevantRels, label));
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
        case FORWARD:
        {
            // Get all relationsFrom ids
            Hyperedges relsFromParents = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfHasA --> X) relations
            Hyperedges relevantRels = intersect(relsFromParents, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::to(relevantRels, label));
        }
        break;
        case BOTH:
        {
            // Get all relationsFrom ids
            Hyperedges relsFromParents = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfHasA --> X) relations
            Hyperedges relevantRels = intersect(relsFromParents, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::to(relevantRels, label));
        }
        case INVERSE:
        {
            // Get all relationsTo id
            Hyperedges relsToParents = Conceptgraph::relationsTo(ids);
            // Contains all (X <-- factFromSubRelOfHasA --> id) relations
            Hyperedges relevantRels = intersect(relsToParents, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::from(relevantRels, label));
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
        case FORWARD:
        {
            // Get all relationsFrom ids
            Hyperedges relsFromConnectors = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfHasA --> X) relations
            Hyperedges relevantRels = intersect(relsFromConnectors, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::to(relevantRels, label));
        }
        break;
        case BOTH:
        {
            // Get all relationsFrom ids
            Hyperedges relsFromConnectors = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfHasA --> X) relations
            Hyperedges relevantRels = intersect(relsFromConnectors, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::to(relevantRels, label));
        }
        case INVERSE:
        {
            // Get all relationsTo id
            Hyperedges relsToConnectors = Conceptgraph::relationsTo(ids);
            // Contains all (X <-- factFromSubRelOfHasA --> id) relations
            Hyperedges relevantRels = intersect(relsToConnectors, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::from(relevantRels, label));
        }
        break;
    }
    return result;
}
