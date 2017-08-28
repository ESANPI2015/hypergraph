#include "Hypergraph.hpp"

#include <iostream>
#include <stack>

Hypergraph::Hypergraph()
{
}

Hypergraph::Hypergraph(Hypergraph& A, Hypergraph& B)
{
    // If things have the same ID they are the same!
    Hyperedges allOfA = A.find();
    Hyperedges allOfB = B.find();

    // First pass: Clone hedges of A and B
    for (auto idA : allOfA)
    {
        create(idA, A.get(idA)->label());
    }
    for (auto idB : allOfB)
    {
        create(idB, B.get(idB)->label());
    }

    // Second pass: Wire the hedges
    Hyperedges allOfMe = find();
    for (auto id : allOfMe)
    {
        // The new hedges will point to/from the union of the corresponding sets of the A and B hedges with the same id
        Hyperedges fromA = A.get(id) ? A.get(id)->pointingFrom() : Hyperedges();
        Hyperedges fromB = B.get(id) ? B.get(id)->pointingFrom() : Hyperedges();
        Hyperedges toA = A.get(id) ? A.get(id)->pointingTo() : Hyperedges();
        Hyperedges toB = B.get(id) ? B.get(id)->pointingTo() : Hyperedges();
        from(unite(fromA, fromB), id);
        to(id, unite(toA, toB));
    }
}

Hypergraph::~Hypergraph()
{
    // We hold no pointers so we do not need to do anything here
}

Hyperedges Hypergraph::create(const unsigned id, const std::string& label)
{
    Hyperedge* neu = get(id);
    if (!neu)
    {
        // Create a new hyperedge
        // Give it the desired id
        _edges[id] = Hyperedge(id, label);
        return Hyperedges{id};
    }
    return Hyperedges();
}

void Hypergraph::destroy(const unsigned id)
{
    auto edge = get(id);
    if (!edge)
        return;

    // disconnect from all other edges
    disconnect(id);

    // delete from repository
    if (_edges.count(id))
    {
        _edges.erase(id);
    }
}

void Hypergraph::disconnect(const unsigned id)
{
    // We have to find all edges referring to us!
    Hyperedges all = find();
    for (auto otherId : all)
    {
        auto other = get(otherId);
        if (other->isPointingTo(id))
        {
            // remove id from _to set
            other->_to.erase(id);
        }
        if (other->isPointingFrom(id))
        {
            // remove id from _from set
            other->_from.erase(id);
        }
    }
}

Hyperedge* Hypergraph::get(const unsigned id)
{
    if (_edges.count(id))
    {
        return &_edges[id];
    } else {
        return NULL;
    }
}

Hyperedges Hypergraph::find(const std::string& label)
{
    Hyperedges result;
    for (auto pair : _edges)
    {
        auto id = pair.first;
        auto edge = pair.second;
        // If edge does not match the label, skip it
        if (!label.empty() && (edge.label() != label))
            continue;
        result.insert(id);
    }
    return result;
}


Hyperedges Hypergraph::to(const unsigned srcId, const unsigned destId)
{
    auto srcEdge = get(srcId);
    auto destEdge = get(destId);
    if (!srcEdge || !destEdge)
        return Hyperedges();
    srcEdge->to(destId);
    return Hyperedges{srcId, destId};
}

Hyperedges Hypergraph::to(const unsigned srcId, const Hyperedges& otherIds)
{
    auto srcEdge = get(srcId);
    if (!srcEdge)
        return Hyperedges();
    for (auto otherId : otherIds)
    {
        // Check if other is part of this graph as well
        auto other = get(otherId);
        if (!other)
            return Hyperedges();
        srcEdge->to(otherId);
    }
    return unite(Hyperedges{srcId}, otherIds);
}

Hyperedges Hypergraph::from(const unsigned srcId, const unsigned destId)
{
    auto srcEdge = get(srcId);
    auto destEdge = get(destId);
    if (!srcEdge || !destEdge)
        return Hyperedges();
    destEdge->from(srcId);
    return Hyperedges{srcId, destId};
}

Hyperedges Hypergraph::from(const Hyperedges& otherIds, const unsigned destId)
{
    auto destEdge = get(destId);
    if (!destEdge)
        return Hyperedges();
    for (auto otherId : otherIds)
    {
        // Check if other is part of this graph as well
        auto other = get(otherId);
        if (!other)
            return Hyperedges();
        destEdge->from(otherId);
    }
    return unite(Hyperedges{destId}, otherIds);
}

Hyperedges Hypergraph::from(const unsigned id, const std::string& label)
{
    Hyperedges result;
    Hyperedges fromIds = get(id)->pointingFrom();
    for (auto fromId : fromIds)
    {
        if (label.empty() || (get(fromId)->label() == label))
            result.insert(fromId);
    }
    return result;
}

Hyperedges Hypergraph::to(const unsigned id, const std::string& label)
{
    Hyperedges result;
    Hyperedges toIds = get(id)->pointingTo();
    for (auto toId : toIds)
    {
        if (label.empty() || (get(toId)->label() == label))
            result.insert(toId);
    }
    return result;
}

Hyperedges Hypergraph::from(const Hyperedges& ids, const std::string& label)
{
    Hyperedges result;
    for (auto id : ids)
    {
        auto fromIds = from(id, label);
        result.insert(fromIds.begin(), fromIds.end());
    }
    return result;
}

Hyperedges Hypergraph::to(const Hyperedges& ids, const std::string& label)
{
    Hyperedges result;
    for (auto id : ids)
    {
        auto toIds = to(id, label);
        result.insert(toIds.begin(), toIds.end());
    }
    return result;
}

Hyperedges Hypergraph::prevNeighboursOf(const unsigned id, const std::string& label)
{
    Hyperedges result;
    result = from(id,label);
    Hyperedges all = find();
    for (unsigned other : all)
    {
        // Label matching
        if (!label.empty() && (get(other)->label() != label))
            continue;
        // Check if id is in the TO set of other
        if (get(other)->isPointingTo(id))
            result.insert(other);
    }
    return result;
}

Hyperedges Hypergraph::prevNeighboursOf(const Hyperedges& ids, const std::string& label)
{
    Hyperedges result;
    for (unsigned id : ids)
    {
        Hyperedges n = prevNeighboursOf(id, label);
        result.insert(n.begin(), n.end());
    }
    return result;
}

Hyperedges Hypergraph::nextNeighboursOf(const unsigned id, const std::string& label)
{
    Hyperedges result;
    result = to(id,label);
    Hyperedges all = find();
    for (unsigned other : all)
    {
        // Label matching
        if (!label.empty() && (get(other)->label() != label))
            continue;
        // Check if id is in the FROM set of other
        if (get(other)->isPointingFrom(id))
            result.insert(other);
    }
    return result;
}

Hyperedges Hypergraph::nextNeighboursOf(const Hyperedges& ids, const std::string& label)
{
    Hyperedges result;
    for (unsigned id : ids)
    {
        Hyperedges n = nextNeighboursOf(id, label);
        result.insert(n.begin(), n.end());
    }
    return result;
}

Hyperedges Hypergraph::allNeighboursOf(const unsigned id, const std::string& label)
{
    Hyperedges result;
    result = unite(prevNeighboursOf(id,label), nextNeighboursOf(id,label));
    return result;
}

Hyperedges Hypergraph::allNeighboursOf(const Hyperedges& ids, const std::string& label)
{
    Hyperedges result;
    for (unsigned id : ids)
    {
        Hyperedges n = allNeighboursOf(id, label);
        result.insert(n.begin(), n.end());
    }
    return result;
}

Mapping Hypergraph::match(const Hyperedges& otherIds)
{
    // This algorithm is according to Ullmann
    // and has been implemented following "An In-depth Comparison of Subgraph Isomorphism Algorithms in Graph Databases"
    // First step: For each vertex in subgraph, we find other suitable candidates
    Mapping currentMapping;
    std::map< unsigned, Hyperedges > candidateIds;
    for (unsigned otherId : otherIds)
    {
        candidateIds[otherId] = find(get(otherId)->label());
        candidateIds[otherId].erase(otherId); // Prevent finding the trivial case
        if (!candidateIds[otherId].size())
            return currentMapping;
    }

    // Second step: Find possible mapping(s)
    // We need a stack of mappings to prevent recursion
    std::stack< Mapping > mappings;
    mappings.push(Mapping());
    while (!mappings.empty())
    {
        // Get top of stack
        currentMapping = mappings.top();
        mappings.pop();

        // Check if we can stop the search
        if (currentMapping.size() == otherIds.size())
        {
            return currentMapping;
        }

        // Otherwise search for a possible new mapping and proceed
        unsigned unmappedId = 0;
        for (unsigned otherId : otherIds)
        {
            unmappedId = otherId;
            if (!currentMapping.count(otherId))
                break;
        }

        // Found unmapped hedge
        Hyperedges candidates = candidateIds[unmappedId];
        Hyperedges nextNeighbourIds = intersect(nextNeighboursOf(unmappedId), otherIds); // Check ONLY the neighbourhood INSIDE the subgraph
        Hyperedges prevNeighbourIds = intersect(prevNeighboursOf(unmappedId), otherIds);
        for (unsigned candidateId : candidates)
        {
            Hyperedges nextCandidateNeighbours = nextNeighboursOf(candidateId);
            Hyperedges prevCandidateNeighbours = prevNeighboursOf(candidateId);
            // Ignore all candidates whose neighbourhood is smaller
            if (nextCandidateNeighbours.size() < nextNeighbourIds.size())
                continue;
            if (prevCandidateNeighbours.size() < prevNeighbourIds.size())
                continue;
            bool foundMatch = true;
            for (unsigned nextNeighbourId : nextNeighbourIds)
            {
                // Check if neighbour is already matched and if its match is also a neighbour of the candidate
                if (currentMapping.count(nextNeighbourId) && !nextCandidateNeighbours.count(currentMapping[nextNeighbourId]))
                {
                    foundMatch = false;
                    break;
                }
            }
            for (unsigned prevNeighbourId : prevNeighbourIds)
            {
                // Check if neighbour is already matched and if its match is also a neighbour of the candidate
                if (currentMapping.count(prevNeighbourId) && !prevCandidateNeighbours.count(currentMapping[prevNeighbourId]))
                {
                    foundMatch = false;
                    break;
                }
            }
            if (foundMatch)
            {
                // Insert match
                Mapping newMapping(currentMapping);
                newMapping[unmappedId] = candidateId;
                mappings.push(newMapping);
            }
        }
    }
    return Mapping();
}

Mapping Hypergraph::rewrite(Mapping& matched, Mapping& replacements)
{
    // matched contains a mapping from a model subgraph to some ismorphism of it in the overall graph
    // replacements contains a mapping from the same model subgraph to another (sub)graph
    Mapping result;

    // Since we know that matched is a one-to-one mapping, we can invert it
    Mapping matchedInv;
    for (const auto &pair : matched)
    {
        matchedInv[pair.second] = pair.first;
    }
    // Do this also for replacements
    Mapping replacementsInv;
    for (const auto &pair : replacements)
    {
        replacementsInv[pair.second] = pair.first;
    }

    // Cycle through all entries of matched
    for (const auto &pair : matched)
    {
        unsigned modelId = pair.first;
        unsigned originalId = pair.second;
        unsigned replacementId = 0;
        if (replacements.count(modelId))
            replacementId = replacements[modelId];

        // Register the change
        result[originalId] = replacementId;

        // If the replacementId is not given/is zero, we delete the original hyperedge and proceed
        if (!replacementId)
        {
            destroy(originalId);
            continue;
        }

        // If the replacement hedge provides a label, we change the label of the original
        if (!get(replacementId)->label().empty())
        {
            get(originalId)->updateLabel(get(replacementId)->label());
        }

        // Now the trickiest part: For all links from/to original and some other original, we have to check if this link also exists between the corresponding replacements
        // Furthermore, we have to check also if a link between replacements are new!
        Hyperedges originalPointsTo = to(originalId);
        Hyperedges replacementPointsTo = to(replacementId);
        // Check (original -> originalOther) -> (replacement -> replacementOther)
        for (unsigned originalOtherId : originalPointsTo)
        {
            // Check if originalOther has also been matched
            if (!matchedInv.count(originalOtherId))
                continue;
            unsigned matchedOtherId = matchedInv[originalOtherId];
            // Check if there is a replacement for originalOther
            if (!replacements.count(matchedOtherId))
                continue;
            unsigned replacementOtherId = replacements[matchedOtherId];
            // Now we have to check if (originalId -> originalOtherId) -> (replacementId -> replacementOtherId) is true
            if (replacementPointsTo.count(replacementOtherId))
                continue;
            // If not, we have to remove (originalId -> originalOtherId)
            get(originalId)->_to.erase(originalOtherId);
        }
        // Check (original -> originalOther) <- (replacement -> replacementOther)
        for (unsigned replacementOtherId : replacementPointsTo)
        {
            // Get the matched id
            if (!replacementsInv.count(replacementOtherId))
            {
                // TODO: If this node can not be found, we have to create it and let original point to it!
                continue;
            }
            unsigned matchedOtherId = replacementsInv[replacementOtherId];
            // Check if there is a match
            if (!matched.count(matchedOtherId))
            {
                // TODO: It would be a serious error of the matching algorithm if we end up here!
                continue;
            }
            unsigned originalOtherId = matched[matchedOtherId];
            // Now we have to check if (originalId -> originalOtherId) <- (replacementId -> replacementOtherId) is true
            if (originalPointsTo.count(originalOtherId))
                continue;
            // If not, we have to add (originalId -> originalOtherId)
            get(originalId)->_to.insert(originalOtherId);
        }
        // Do the same with the from set
        Hyperedges originalPointsFrom = from(originalId);
        Hyperedges replacementPointsFrom = from(replacementId);
        // Check (originalOther <- original) -> (replacementOther <- replacement)
        for (unsigned originalOtherId : originalPointsFrom)
        {
            // Check if originalOther has also been matched
            if (!matchedInv.count(originalOtherId))
                continue;
            unsigned matchedOtherId = matchedInv[originalOtherId];
            // Check if there is a replacement for originalOther
            if (!replacements.count(matchedOtherId))
                continue;
            unsigned replacementOtherId = replacements[matchedOtherId];
            // Now we have to check if (originalOtherId <- originalId) -> (replacementOtherId <- replacementId) is true
            if (replacementPointsFrom.count(replacementOtherId))
                continue;
            // If not, we have to remove (originalOtherId <- originalId)
            get(originalId)->_from.erase(originalOtherId);
        }
        // Check (originalOther <- original) <- (replacementOther <- replacement)
        for (unsigned replacementOtherId : replacementPointsFrom)
        {
            // Get the matched id
            if (!replacementsInv.count(replacementOtherId))
            {
                // TODO: If this node can not be found, we have to create it and let original point from it!
                continue;
            }
            unsigned matchedOtherId = replacementsInv[replacementOtherId];
            // Check if there is a match
            if (!matched.count(matchedOtherId))
            {
                // TODO: It would be a serious error of the matching algorithm if we end up here!
                continue;
            }
            unsigned originalOtherId = matched[matchedOtherId];
            // Now we have to check if (originalOtherId <- originalId) <- (replacementOtherId <- replacementId) is true
            if (originalPointsFrom.count(originalOtherId))
                continue;
            // If not, we have to add (originalOtherId <- originalId)
            get(originalId)->_from.insert(originalOtherId);
        }
    }
    return result;
}
