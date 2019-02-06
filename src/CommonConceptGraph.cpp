#include "CommonConceptGraph.hpp"
#include <iostream>

const UniqueId CommonConceptGraph::FactOfId = "CommonConceptGraph::FactOf";
const UniqueId CommonConceptGraph::SubrelOfId = "CommonConceptGraph::SubrelOf";
const UniqueId CommonConceptGraph::IsAId = "CommonConceptGraph::IsA";
const UniqueId CommonConceptGraph::HasAId = "CommonConceptGraph::HasA";
const UniqueId CommonConceptGraph::PartOfId = "CommonConceptGraph::PartOf";
const UniqueId CommonConceptGraph::ConnectsId = "CommonConceptGraph::Connects";
const UniqueId CommonConceptGraph::InstanceOfId = "CommonConceptGraph::InstanceOf";

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

void CommonConceptGraph::createCommonConcepts()
{
    /* Common relations over relations*/
    if (!exists(CommonConceptGraph::FactOfId))
    {
        Conceptgraph::relate(CommonConceptGraph::FactOfId, Hyperedges{Conceptgraph::IsRelationId}, Hyperedges{Conceptgraph::IsRelationId}, "FACT-OF");
        // Make factOf point from and to itself
        access(CommonConceptGraph::FactOfId).pointsFrom(CommonConceptGraph::FactOfId);
        access(CommonConceptGraph::FactOfId).pointsTo(CommonConceptGraph::FactOfId);
    }
    Conceptgraph::relate(CommonConceptGraph::SubrelOfId, Hyperedges{Conceptgraph::IsRelationId}, Hyperedges{Conceptgraph::IsRelationId}, "SUBREL-OF");

    /* Common relations over concepts*/
    Conceptgraph::relate(CommonConceptGraph::IsAId, Hyperedges{Conceptgraph::IsConceptId}, Hyperedges{Conceptgraph::IsConceptId}, "IS-A");
    Conceptgraph::relate(CommonConceptGraph::HasAId, Hyperedges{Conceptgraph::IsConceptId}, Hyperedges{Conceptgraph::IsConceptId}, "HAS-A");
    Conceptgraph::relate(CommonConceptGraph::PartOfId, Hyperedges{Conceptgraph::IsConceptId}, Hyperedges{Conceptgraph::IsConceptId}, "PART-OF");
    Conceptgraph::relate(CommonConceptGraph::ConnectsId, Hyperedges{Conceptgraph::IsConceptId}, Hyperedges{Conceptgraph::IsConceptId}, "CONNECTS");
    Conceptgraph::relate(CommonConceptGraph::InstanceOfId, Hyperedges{Conceptgraph::IsConceptId}, Hyperedges{Conceptgraph::IsConceptId}, "INSTANCE-OF");
}


Hyperedges CommonConceptGraph::factOf(const Hyperedges& factIds, const Hyperedges& superRelIds)
{
    Hyperedges ids;
    Hyperedges fromIds = intersect(Conceptgraph::relations(), factIds);
    Hyperedges toIds = intersect(Conceptgraph::relations(), superRelIds);
    // Bacause FACT-OF is a BINARY relation we have to create one entity per pair
    for (const UniqueId& fromId : fromIds)
    {
        for (const UniqueId& toId : toIds)
        {
            Hyperedges id(Conceptgraph::relateFrom(Hyperedges{fromId}, Hyperedges{toId}, CommonConceptGraph::FactOfId));
            if (!id.empty())
            {
                // If we have created a factOf a factOf superrelation :D , we have to link them
                Hypergraph::pointsFrom(Hyperedges{CommonConceptGraph::FactOfId}, id);
                ids = unite(ids, id);
            }
        }
    }
    return ids;
}

Hyperedges CommonConceptGraph::factFrom(const Hyperedges& fromIds, const Hyperedges& toIds, const UniqueId superId)
{
    // At first create the relation ...
    Hyperedges id;
    // When we create a FACT of a RELATION, we have to check that in- and outdegree match!
    // FIXME: We should also check if a) fromIds is a subclass, subrelation, fact or instance of superId.from b) toIds is a subclass, subrelation, fact or instance of superId.to
    if ((fromIds.size() == access(superId).indegree()) && (toIds.size() == access(superId).outdegree()))
    {
        id = Conceptgraph::relateFrom(fromIds, toIds, superId);
        if (!id.empty())
        {
            // ... then make the new relation a factOf the super relation
            factOf(id, Hyperedges{superId});
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
        Hyperedges superRels(factsOf(otherId, Hyperedges(), Hyperedges(), TraversalDirection::FORWARD));
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
        // NOTE: This will fail if fromIds.size() and toIds.size() do not match the in- and outdegree of SUBREL-OF (which are 1,1)
        id = factFrom(fromIds, toIds, CommonConceptGraph::SubrelOfId);
    }
    return id;
}

Hyperedges CommonConceptGraph::subrelationFrom(const UniqueId& subRelId, const Hyperedges& fromIds, const Hyperedges& toIds, const UniqueId& superRelId)
{
    Hyperedges id;
    // When we create a SUBRELATION of a RELATION, we have to check that arity matches
    // FIXME:  fromIds must be a subclass or subrelation of superRelId.from and toIds must be a subclass or subrelation of superRelId.to
    if ((fromIds.size() == access(superRelId).indegree()) && (toIds.size() == access(superRelId).outdegree()))
    {
        id = Conceptgraph::relateFrom(subRelId, fromIds, toIds, superRelId);
        if (!id.empty())
        {
            // ... then make the new relation a subrelation of the superrelation
            subrelationOf(id, Hyperedges{superRelId});
        }
    }
    return id;
}

Hyperedges CommonConceptGraph::isA(const Hyperedges& subIds, const Hyperedges& superIds)
{
    Hyperedges result;
    for (const UniqueId& subId : subIds)
    {
        for (const UniqueId& superId : superIds)
        {
            result = unite(result, factFrom(Hyperedges{subId}, Hyperedges{superId}, CommonConceptGraph::IsAId));
        }
    }
    return result;
}

Hyperedges CommonConceptGraph::hasA(const Hyperedges& parentIds, const Hyperedges& childIds)
{
    Hyperedges result;
    for (const UniqueId& pId : parentIds)
    {
        for (const UniqueId& cId : childIds)
        {
            result = unite(result, factFrom(Hyperedges{pId}, Hyperedges{cId}, CommonConceptGraph::HasAId));
        }
    }
    return result;
}

Hyperedges CommonConceptGraph::partOf(const Hyperedges& partIds, const Hyperedges& wholeIds)
{
    Hyperedges result;
    for (const UniqueId& pId : partIds)
    {
        for (const UniqueId& wId : wholeIds)
        {
            result = unite(result, factFrom(Hyperedges{pId}, Hyperedges{wId}, CommonConceptGraph::PartOfId));
        }
    }
    return result;
}

Hyperedges CommonConceptGraph::connects(const Hyperedges& connectorIds, const Hyperedges& interfaceIds)
{
    Hyperedges result;
    for (const UniqueId& cId : connectorIds)
    {
        for (const UniqueId& iId : interfaceIds)
        {
            result = unite(result, factFrom(Hyperedges{cId}, Hyperedges{iId}, CommonConceptGraph::ConnectsId));
        }
    }
    return result;
}

Hyperedges CommonConceptGraph::instanceOf(const Hyperedges& individualIds, const Hyperedges& superIds)
{
    Hyperedges result;
    for (const UniqueId& iId : individualIds)
    {
        for (const UniqueId& sId : superIds)
        {
            result = unite(result, factFrom(Hyperedges{iId}, Hyperedges{sId}, CommonConceptGraph::InstanceOfId));
        }
    }
    return result;
}

Hyperedges CommonConceptGraph::createSubclassOf(const UniqueId& subId, const Hyperedges& superIds, const std::string& label)
{
    Hyperedges result(Conceptgraph::concept(subId, label));
    isA(result, superIds);
    return result;
}

Hyperedges CommonConceptGraph::instantiateFrom(const UniqueId superId, const std::string& label)
{
    std::string theLabel(label);
    if (theLabel.empty())
    {
        theLabel = Hypergraph::access(superId).label();
    }

    UniqueId id(superId);
    while (Conceptgraph::concept(id, theLabel).empty())
    {
        auto myHash(std::hash<UniqueId>{}(id));
        auto newHash(std::hash<UniqueId>{}(theLabel));
        id = std::to_string(myHash ^ (newHash << 1));
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
        // If no label is given, clone the label of otherId
        std::string theLabel(label);
        if (theLabel.empty())
        {
            theLabel = Hypergraph::access(otherId).label();
        }
        // Get superclass of other
        Hyperedges superclasses = instancesOf(otherId,"",TraversalDirection::FORWARD);
        // Instantiate from superclass of other
        result = unite(result, instantiateFrom(superclasses, theLabel));
    }
    return result;
}

Hyperedges CommonConceptGraph::factsOf(const UniqueId& superRelId, const Hyperedges& fromIds, const Hyperedges& toIds, const TraversalDirection dir, const std::string& label) const
{
    Hyperedges result;
    Hyperedges allFacts(access(CommonConceptGraph::FactOfId).pointingFrom()); // * <- FACT-OF -> ** ...  Either * or ** have to be intersected depending on dir

    // The direction defines if we return FACTS or RELATION DEFINITIONS
    switch (dir)
    {
        case INVERSE:
            {
                // Since FACT-OF points from a FACT to a RELATION DEF, the inverse dir will return the FACTS
                Hyperedges relationsToSuperRel(Conceptgraph::relationsTo(Hyperedges{superRelId})); // * <- REL -> superRel
                Hyperedges allFactsToSuperRel(intersect(allFacts, relationsToSuperRel)); // * <- FACT-OF -> superRel
                result = unite(result, Hypergraph::isPointingFrom(allFactsToSuperRel));
            }
            break;
        case BOTH:
            {
                // INVERSE first ... then fall through to FORWARD
                Hyperedges relationsToSuperRel(Conceptgraph::relationsTo(Hyperedges{superRelId})); // * <- REL -> superRel
                Hyperedges allFactsToSuperRel(intersect(allFacts, relationsToSuperRel)); // * <- FACT-OF -> superRel
                result = unite(result, Hypergraph::isPointingFrom(allFactsToSuperRel));
            }
        case FORWARD:
            {
                // Now FORWARD. IMPORTANT: Although the variable is calles superRelId, it is treated HERE as factId!
                const UniqueId factId(superRelId);
                Hyperedges relationsFromFact(Conceptgraph::relationsFrom(Hyperedges{factId})); // fact <- REL -> *
                Hyperedges allSuperRelsFromFact(intersect(allFacts, relationsFromFact)); // fact <- FACT-OF -> *
                result = unite(result, Hypergraph::isPointingTo(allSuperRelsFromFact));
            }
            break;
    }

    // In the next step we have to filter the resulting facts or relation definitions by the concepts they relate!
    // If either one (or both) concept sets are empty, we just pass the complete result set.
    Hyperedges relationsFromIds(fromIds.size() > 0 ? Conceptgraph::relationsFrom(fromIds, label) : result); // fromIds <- REL(label) -> *
    Hyperedges relationsToIds(toIds.size() > 0 ? Conceptgraph::relationsTo(toIds, label) : result); // * <- REL(label) -> toIds
    Hyperedges relationsMatchFromAndToIds(intersect(relationsFromIds, relationsToIds)); // fromIds <- REL(label) -> toIds

    return intersect(result, relationsMatchFromAndToIds);
}


Hyperedges CommonConceptGraph::factsOf(const Hyperedges& superRelIds, const Hyperedges& fromIds, const Hyperedges& toIds, const TraversalDirection dir, const std::string& label) const
{
    Hyperedges result;
    for (const UniqueId& superRelId : superRelIds)
    {
        Hyperedges some = factsOf(superRelId, fromIds, toIds, dir, label);
        result.insert(result.end(), some.begin(), some.end());
    }
    return result;
}

Hyperedges CommonConceptGraph::subrelationsOf(const Hyperedges& superRelIds, const std::string& label, const TraversalDirection dir) const
{
    Hyperedges result;
    for (UniqueId superRelId : superRelIds)
        result = unite(result, subrelationsOf(superRelId, label, dir));
    return result;
}

Hyperedges CommonConceptGraph::subrelationsOf(const UniqueId superRelId, const std::string& label, const TraversalDirection dir) const
{
    // Here we start a traversal from superRelId following every subrelationOf relation
    // First, we define the filter functions for the concepts to be returned
    auto cf = [&](const Hyperedge& c) -> bool {
        if (label.empty() || (c.label() == label))
            return true;
        return false;
    };
    // Then, we define the function to decide which relations to follow
    // The criterium is, that there exists r <- FACT-OF -> CommonConceptGraph::SubrelOfId
    auto rf = [&](const Hyperedge& c, const Hyperedge& r) -> bool {
        Hyperedges toSearch(isPointingTo(relationsFrom(Hyperedges{r.id()}, access(CommonConceptGraph::FactOfId).label())));
        if (std::find(toSearch.begin(), toSearch.end(), CommonConceptGraph::SubrelOfId) != toSearch.end())
            return true;
        return false;
    };
    return Conceptgraph::traverse(superRelId, cf, rf, dir);
}

Hyperedges CommonConceptGraph::transitiveClosure(const UniqueId rootId, const UniqueId relId, const std::string& label, const TraversalDirection dir) const
{
    // At first, find all relations we have to consider during traversal:
    // These are all subrelations of relId including relId itself
    Hyperedges relationsToFollow(subrelationsOf(relId));

    // The filter function is like the one in subrelationsOf
    auto cf = [&](const Hyperedge& c) -> bool {
        if (label.empty() || (c.label() == label))
            return true;
        return false;
    };

    // For the relation filter function, we have to check that
    // r <- FACT-OF -> R where R is element of relationsToFollow
    auto rf = [&](const Hyperedge& c, const Hyperedge& r) -> bool {
        Hyperedges toSearch(isPointingTo(relationsFrom(Hyperedges{r.id()}, access(CommonConceptGraph::FactOfId).label())));
        if (intersect(toSearch, relationsToFollow).empty())
            return false;
        return true;
    };

    return Conceptgraph::traverse(rootId, cf, rf, dir);
}

Hyperedges CommonConceptGraph::subclassesOf(const Hyperedges& superIds, const std::string& label, const TraversalDirection dir) const
{
    Hyperedges result;
    for (UniqueId id : superIds)
    {
        result = unite(result, transitiveClosure(id, CommonConceptGraph::IsAId, label, dir));
    }
    return result;
}

Hyperedges CommonConceptGraph::partsOf(const Hyperedges& wholeIds, const std::string& label, const TraversalDirection dir) const
{
    Hyperedges result;
    for (UniqueId id : wholeIds)
    {
        result = unite(result, transitiveClosure(id, CommonConceptGraph::PartOfId, label, dir));
    }
    return result;
}

Hyperedges CommonConceptGraph::descendantsOf(const Hyperedges& ancestorIds, const std::string& label, const TraversalDirection dir) const
{
    Hyperedges result;
    for (UniqueId id : ancestorIds)
    {
        result = unite(result, transitiveClosure(id, CommonConceptGraph::HasAId, label, dir));
    }
    return result;
}

Hyperedges CommonConceptGraph::directSubrelationsOf(const Hyperedges& ids, const std::string& label, const TraversalDirection dir) const
{
    Hyperedges result;
    // Get all factsOf these SUBREL-OF
    Hyperedges facts(factsOf(CommonConceptGraph::SubrelOfId));

    switch (dir)
    {
        case FORWARD:
        {
            // Get all relationsFrom id
            Hyperedges relsFromSubs = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOf --> X) relations
            Hyperedges relevantRels = intersect(relsFromSubs, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::isPointingTo(relevantRels, label));
        }
        break;
        case BOTH:
        {
            // Get all relationsFrom id
            Hyperedges relsFromSubs = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOf --> X) relations
            Hyperedges relevantRels = intersect(relsFromSubs, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::isPointingTo(relevantRels, label));
        }
        case INVERSE:
        {
            // Get all relationsTo id
            Hyperedges relsToSupers = Conceptgraph::relationsTo(ids);
            // Contains all (X <-- factFromSubRelOf --> id) relations
            Hyperedges relevantRels = intersect(relsToSupers, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::isPointingFrom(relevantRels, label));
        }
        break;
    }
    return result;
}

Hyperedges CommonConceptGraph::directSubclassesOf(const Hyperedges& ids, const std::string& label, const TraversalDirection dir) const
{
    Hyperedges result;
    // Get all subrelationsOf IS-A
    Hyperedges subRels(subrelationsOf(CommonConceptGraph::IsAId));
    // Get all factsOf these subrelations
    Hyperedges facts(factsOf(subRels));

    switch (dir)
    {
        case FORWARD:
        {
            // Get all relationsFrom id
            Hyperedges relsFromSubs = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfIsA --> X) relations
            Hyperedges relevantRels = intersect(relsFromSubs, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::isPointingTo(relevantRels, label));
        }
        break;
        case BOTH:
        {
            // Get all relationsFrom id
            Hyperedges relsFromSubs = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfIsA --> X) relations
            Hyperedges relevantRels = intersect(relsFromSubs, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::isPointingTo(relevantRels, label));
        }
        case INVERSE:
        {
            // Get all relationsTo id
            Hyperedges relsToSupers = Conceptgraph::relationsTo(ids);
            // Contains all (X <-- factFromSubRelOfIsA --> id) relations
            Hyperedges relevantRels = intersect(relsToSupers, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::isPointingFrom(relevantRels, label));
        }
        break;
    }
    return result;
}

Hyperedges CommonConceptGraph::instancesOf(const Hyperedges& ids, const std::string& label, const TraversalDirection dir) const
{
    Hyperedges result;
    // Get all subrelationsOf INSTANCE-OF
    Hyperedges subRels(subrelationsOf(CommonConceptGraph::InstanceOfId));
    // Get all factsOf these subrelations
    Hyperedges facts(factsOf(subRels));

    switch (dir)
    {
        case FORWARD:
        {
            // Get all relationsFrom id
            Hyperedges relsFromIndividuals = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfInstanceOf --> X) relations
            Hyperedges relevantRels = intersect(relsFromIndividuals, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::isPointingTo(relevantRels, label));
        }
        break;
        case BOTH:
        {
            // Get all relationsFrom id
            Hyperedges relsFromIndividuals = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfInstanceOf --> X) relations
            Hyperedges relevantRels = intersect(relsFromIndividuals, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::isPointingTo(relevantRels, label));
        }
        case INVERSE:
        {
            // Get all relationsTo id
            Hyperedges relsToSupers = Conceptgraph::relationsTo(ids);
            // Contains all (X <-- factFromSubRelOfInstanceOf --> id) relations
            Hyperedges relevantRels = intersect(relsToSupers, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::isPointingFrom(relevantRels, label));
        }
        break;
    }
    return result;
}

Hyperedges CommonConceptGraph::componentsOf(const Hyperedges& ids, const std::string& label, const TraversalDirection dir) const
{
    Hyperedges result;
    // Get all subrelationsOf partOf
    Hyperedges subRels(subrelationsOf(CommonConceptGraph::PartOfId));
    // Get all factsOf these subrelations
    Hyperedges facts(factsOf(subRels));

    switch (dir)
    {
        case FORWARD:
        {
            // Get all relationsFrom id
            Hyperedges relsFromParts = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfPartOf --> X) relations
            Hyperedges relevantRels = intersect(relsFromParts, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::isPointingTo(relevantRels, label));
        }
        break;
        case BOTH:
        {
            // Get all relationsFrom id
            Hyperedges relsFromParts = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfPartOf --> X) relations
            Hyperedges relevantRels = intersect(relsFromParts, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::isPointingTo(relevantRels, label));
        }
        case INVERSE:
        {
            // Get all relationsTo id
            Hyperedges relsToWholes = Conceptgraph::relationsTo(ids);
            // Contains all (X <-- factFromSubRelOfPartOf --> id) relations
            Hyperedges relevantRels = intersect(relsToWholes, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::isPointingFrom(relevantRels, label));
        }
        break;
    }
    return result;
}

Hyperedges CommonConceptGraph::childrenOf(const Hyperedges& ids, const std::string& label, const TraversalDirection dir) const
{
    Hyperedges result;
    // Get all subrelationsOf HAS-A
    Hyperedges subRels(subrelationsOf(CommonConceptGraph::HasAId));
    // Get all factsOf these subrelations
    Hyperedges facts(factsOf(subRels));

    switch (dir)
    {
        case FORWARD:
        {
            // Get all relationsFrom ids
            Hyperedges relsFromParents = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfHasA --> X) relations
            Hyperedges relevantRels = intersect(relsFromParents, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::isPointingTo(relevantRels, label));
        }
        break;
        case BOTH:
        {
            // Get all relationsFrom ids
            Hyperedges relsFromParents = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfHasA --> X) relations
            Hyperedges relevantRels = intersect(relsFromParents, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::isPointingTo(relevantRels, label));
        }
        case INVERSE:
        {
            // Get all relationsTo id
            Hyperedges relsToParents = Conceptgraph::relationsTo(ids);
            // Contains all (X <-- factFromSubRelOfHasA --> id) relations
            Hyperedges relevantRels = intersect(relsToParents, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::isPointingFrom(relevantRels, label));
        }
        break;
    }
    return result;
}

Hyperedges CommonConceptGraph::endpointsOf(const Hyperedges& ids, const std::string& label, const TraversalDirection dir) const
{
    Hyperedges result;
    // Get all subrelationsOf CONNECTS
    Hyperedges subRels(subrelationsOf(CommonConceptGraph::ConnectsId));
    // Get all factsOf these subrelations
    Hyperedges facts(factsOf(subRels));

    switch (dir)
    {
        case FORWARD:
        {
            // Get all relationsFrom ids
            Hyperedges relsFromConnectors = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfHasA --> X) relations
            Hyperedges relevantRels = intersect(relsFromConnectors, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::isPointingTo(relevantRels, label));
        }
        break;
        case BOTH:
        {
            // Get all relationsFrom ids
            Hyperedges relsFromConnectors = Conceptgraph::relationsFrom(ids);
            // Contains all (id <-- factFromSubRelOfHasA --> X) relations
            Hyperedges relevantRels = intersect(relsFromConnectors, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::isPointingTo(relevantRels, label));
        }
        case INVERSE:
        {
            // Get all relationsTo id
            Hyperedges relsToConnectors = Conceptgraph::relationsTo(ids);
            // Contains all (X <-- factFromSubRelOfHasA --> id) relations
            Hyperedges relevantRels = intersect(relsToConnectors, facts);
            // Get all these X matching the label
            result = unite(result, Hypergraph::isPointingFrom(relevantRels, label));
        }
        break;
    }
    return result;
}
