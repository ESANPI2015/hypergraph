#include "Hypergraph.hpp"

#include <iostream>

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
        from(unite(fromA, fromB), Hyperedges{id});
        to(Hyperedges{id}, unite(toA, toB));
    }
}

Hypergraph::~Hypergraph()
{
    // We hold no pointers so we do not need to do anything here
}

Hyperedges Hypergraph::create(const UniqueId id, const std::string& label)
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

void Hypergraph::destroy(const UniqueId id)
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

void Hypergraph::disconnect(const UniqueId id)
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

Hyperedge* Hypergraph::get(const UniqueId id)
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


Hyperedges Hypergraph::from(const Hyperedges& otherIds, const Hyperedges& destIds)
{
    Hyperedges result;
    for (auto destId : destIds)
    {
        auto destEdge = get(destId);
        if (!destEdge)
            continue;
        for (auto otherId : otherIds)
        {
            // Check if other is part of this graph as well
            auto other = get(otherId);
            if (!other)
                continue;
            destEdge->from(otherId);
            // On success, register that pair
            result = unite(result, Hyperedges{destId, otherId});
        }
    }
    return result;
}

Hyperedges Hypergraph::from(const Hyperedges& ids, const std::string& label)
{
    Hyperedges result;
    for (auto id : ids)
    {
        auto fromIds = get(id)->pointingFrom();
        for (auto fromId : fromIds)
        {
            if (label.empty() || (get(fromId)->label() == label))
                result.insert(fromId);
        }
    }
    return result;
}

Hyperedges Hypergraph::to(const Hyperedges& srcIds, const Hyperedges& otherIds)
{
    Hyperedges result;
    for (auto srcId : srcIds)
    {
        auto srcEdge = get(srcId);
        if (!srcEdge)
            continue;
        for (auto otherId : otherIds)
        {
            // Check if other is part of this graph as well
            auto other = get(otherId);
            if (!other)
                continue;
            srcEdge->to(otherId);
            // On success, register that pair
            result = unite(result, Hyperedges{srcId, otherId});
        }
    }
    return result;
}

Hyperedges Hypergraph::to(const Hyperedges& ids, const std::string& label)
{
    Hyperedges result;
    for (auto id : ids)
    {
        auto fromIds = get(id)->pointingTo();
        for (auto fromId : fromIds)
        {
            if (label.empty() || (get(fromId)->label() == label))
                result.insert(fromId);
        }
    }
    return result;
}

Hyperedges Hypergraph::prevNeighboursOf(const Hyperedges& ids, const std::string& label)
{
    Hyperedges result;
    Hyperedges all = find(label);
    for (UniqueId id : ids)
    {
        result = unite(result, from(Hyperedges{id},label));
        for (UniqueId other : all)
        {
            // Check if id is in the TO set of other
            if (get(other)->isPointingTo(id))
                result.insert(other);
        }
    }
    return result;
}

Hyperedges Hypergraph::nextNeighboursOf(const Hyperedges& ids, const std::string& label)
{
    Hyperedges result;
    Hyperedges all = find(label);
    for (UniqueId id : ids)
    {
        result = unite(result, to(Hyperedges{id},label));
        for (UniqueId other : all)
        {
            // Check if id is in the TO set of other
            if (get(other)->isPointingFrom(id))
                result.insert(other);
        }
    }
    return result;
}

Hyperedges Hypergraph::allNeighboursOf(const Hyperedges& ids, const std::string& label)
{
    Hyperedges result;
    result = unite(prevNeighboursOf(ids,label), nextNeighboursOf(ids,label));
    return result;
}

std::ostream& operator<< (std::ostream& os , const Mapping& val)
{
    os << "{ ";
    for (const auto &pair : val)
    {
        os << "(" << pair.first << " , " << pair.second << ") ";
    }
    os << "} ";
    return os;
}

Mapping fromHyperedges(const Hyperedges& a)
{
    Mapping result;
    for (UniqueId id : a)
    {
        result[id] = id;
    }
    return result;
}

bool equal(const Mapping& a, const Mapping& b)
{
    // For two mappings to be equal, they
    // a) have to have the same size
    if (a.size() != b.size())
        return false;
    // b) for each pair in a:
    for (const auto& pair : a)
    {
        Mapping::const_iterator otherPairIt(b.find(pair.first));
        // b1) a.first in b?
        if (otherPairIt == b.end())
            return false;
        // b2) a.second == b[a.first]?
        if (otherPairIt->second != pair.second)
            return false;
    }
    return true;
}

Mapping Hypergraph::match(Hypergraph& other, std::stack< Mapping >& searchSpace)
{
    // This algorithm is according to Ullmann
    // and has been implemented following "An In-depth Comparison of Subgraph Isomorphism Algorithms in Graph Databases"
    // First step: For each vertex in subgraph, we find other suitable candidates
    Mapping currentMapping;
    std::map< UniqueId, Hyperedges > candidateIds;
    Hyperedges otherIds = other.find();
    for (UniqueId otherId : otherIds)
    {
        // Filter by label
        // NOTE: Do not prevent the trivial case here!!!
        Hyperedges filteredByLabel = find(other.get(otherId)->label());
        // Filter by degree
        // Check in and out degrees here as well!! If candidate has LESS in or out degree it can not be a candidate
        unsigned otherIndegree = other.get(otherId)->indegree();
        unsigned otherOutdegree = other.get(otherId)->outdegree();
        Hyperedges filteredByLabelAndDegree(filteredByLabel);
        for (UniqueId candidateId : filteredByLabel)
        {
            if (get(candidateId)->indegree() < otherIndegree)
                filteredByLabelAndDegree.erase(candidateId);
            if (get(candidateId)->outdegree() < otherOutdegree)
                filteredByLabelAndDegree.erase(candidateId);
        }
        // Check if solution possible
        if (!filteredByLabelAndDegree.size())
            return currentMapping;
        candidateIds[otherId] = filteredByLabelAndDegree;
    }

    // Second step: Find possible mapping(s)
    // We need a stack of mappings to prevent recursion
    if (searchSpace.empty())
        searchSpace.push(Mapping());
    while (!searchSpace.empty())
    {
        // Get top of stack
        currentMapping = searchSpace.top();
        searchSpace.pop();

        // For a correct mapping we have to check if all from and to sets are correct (similar to the check in rewrite)
        bool valid = true;
        for (const auto& pair : currentMapping)
        {
            Hyperedges templatePointsTo(other.to(Hyperedges{pair.first}));
            Hyperedges templatePointsFrom(other.from(Hyperedges{pair.first}));
            Hyperedges matchPointsTo(to(Hyperedges{pair.second}));
            Hyperedges matchPointsFrom(from(Hyperedges{pair.second}));
            for (UniqueId templateId : templatePointsTo)
            {
                if (!currentMapping.count(templateId))
                    continue;
                UniqueId matchId = currentMapping[templateId];
                if (!matchPointsTo.count(matchId))
                {
                    valid = false;
                    break;
                }
            }
            if (!valid)
                break;
            for (UniqueId templateId : templatePointsFrom)
            {
                if (!currentMapping.count(templateId))
                    continue;
                UniqueId matchId = currentMapping[templateId];
                if (!matchPointsFrom.count(matchId))
                {
                    valid = false;
                    break;
                }
            }
            if (!valid)
                break;
        }
        if (!valid)
            continue;

        // Check if we can stop the search
        if (currentMapping.size() == otherIds.size())
        {
            return currentMapping;
        }

        // Otherwise search for a possible new mapping and proceed
        UniqueId unmappedId;
        for (UniqueId otherId : otherIds)
        {
            unmappedId = otherId;
            if (!currentMapping.count(otherId))
                break;
        }

        // Found unmapped hedge
        // NOTE: This is actually what makes this method an Ullmann algorithm
        Hyperedges candidates = candidateIds[unmappedId];
        Hyperedges nextNeighbourIds = other.to(Hyperedges{unmappedId}); // Check ONLY the neighbourhood INSIDE the subgraph
        Hyperedges prevNeighbourIds = other.from(Hyperedges{unmappedId});
        for (UniqueId candidateId : candidates)
        {
            Hyperedges nextCandidateNeighbours = to(Hyperedges{candidateId});
            Hyperedges prevCandidateNeighbours = from(Hyperedges{candidateId});
            // Ignore all candidates whose neighbourhood is smaller
            if (nextCandidateNeighbours.size() < nextNeighbourIds.size())
                continue;
            if (prevCandidateNeighbours.size() < prevNeighbourIds.size())
                continue;
            bool foundMatch = true;
            for (UniqueId nextNeighbourId : nextNeighbourIds)
            {
                // Check if neighbour is already matched and if its match is also a neighbour of the candidate
                if (currentMapping.count(nextNeighbourId) && !nextCandidateNeighbours.count(currentMapping[nextNeighbourId]))
                {
                    foundMatch = false;
                    break;
                }
            }
            for (UniqueId prevNeighbourId : prevNeighbourIds)
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
                searchSpace.push(newMapping);
            }
        }
    }
    return Mapping();
}

Mapping invert(const Mapping& m)
{
    Mapping result;
    for (const auto &pair : m)
    {
        result[pair.second] = pair.first;
    }
    return result;
}

Mapping join(const Mapping& a, const Mapping& b)
{
    Mapping result;
    for (const auto &pair : a)
    {
        // Find occurrence in b
        Mapping::const_iterator otherPairIt(b.find(pair.first));
        if (otherPairIt == b.end())
            continue;
        // If found make map with a.second -> b.second
        result[pair.second] = otherPairIt->second;
    }
    return result;
}

/*
* This algorithm is a single pushout (SPO) graph transformation algorithm
*/
Hypergraph Hypergraph::rewrite(Hypergraph& lhs, Hypergraph& rhs, const Mapping& partialMap)
{
    Hypergraph result;
    Mapping original2new;
    Mapping replacement2new;

    // First step: Find a match of lhs in this graph
    std::stack< Mapping > sp;
    Mapping m(match(lhs, sp));
    // No match? Return empty graph
    if (!m.size())
        return result;

    // Second step: recreate all HYPEREDGES which are NOT PART of the match! (G\lhs)
    Hyperedges originals(find());
    Mapping mInv(invert(m));
    Hyperedges preservedOriginals;
    for (const UniqueId originalId : originals)
    {
        if (mInv.count(originalId))
            continue;
        // Does not exist for sure, so will always succeed
        result.create(originalId, get(originalId)->label());
        preservedOriginals.insert(originalId);
        original2new[originalId] = originalId;
    }

    // Third step: Cycle over matches
    // Here, we either delete OR preserve & alter originals
    // NOTE: Deletion here is IMPLICIT (by not creating it)
    Hyperedges alteredOriginals;
    for (const auto &pair : m)
    {
        UniqueId matchedId(pair.first);
        UniqueId originalId(pair.second);
        UniqueId replacementId;
        if (partialMap.count(matchedId))
        {
            // Found a replacement for original
            replacementId = partialMap.at(matchedId);
        }
        //std::cout << originalId << " <- " << matchedId << " -> " << replacementId << "\n";
        // Check replacementId
        if (!replacementId.empty())
        {
            // Assign the label given by the replacement if it is not empty
            std::string label(rhs.get(replacementId)->label());
            if (label.empty())
            {
                label = get(originalId)->label();
            }
            // Tricky: The original shall be preserved, but two orignal can point to the same replacement!!! What does that mean?
            // It could mean, that they both get the same label but different UIDs or they are replaced by the same UID ...
            // Intuitively I'd expect two hedges to be merged to ONE
            // However we should preserve at least one of the original UIDs (they might encode important info like LOCATION or such)
            UniqueId uid = originalId;
            if (replacement2new.count(replacementId))
            {
                // If an original already has been altered by the same replacement UID, we reuse the UID of that original
                uid = replacement2new[replacementId];
            }
            // It might be that a hedge with uid already exists, if it does, we will reuse it
            result.create(uid, label);
            original2new[originalId] = uid;
            replacement2new[replacementId] = uid;
            alteredOriginals.insert(originalId);
        }
    }

    // Fourth step: Now all hedges in rhs which are not in result yet, have to be created
    Hyperedges replacements(rhs.find());
    Mapping rInv(invert(partialMap));
    for (const UniqueId replacementId : replacements)
    {
        if (rInv.count(replacementId))
            continue;
        // Create the new hedge. If uid is already taken, assign a new one
        // TODO: Check if this is what we want ... maybe two things with the same UID should become the same afterwards
        unsigned occurrence = 1;
        UniqueId uid = replacementId;
        while (result.create(uid, rhs.get(replacementId)->label()).empty())
            uid = replacementId + std::to_string(occurrence++);
        replacement2new[replacementId] = uid;
    }

    // Fifth step: Wiring
    // A) Reconstruct wiring of original hedges
    Hyperedges handledEdges;
    for (const UniqueId firstIdOld : originals)
    {
        // Check if also part of the result
        if (!original2new.count(firstIdOld))
            continue;
        // Get new uid
        UniqueId firstId = original2new[firstIdOld];
        for (const UniqueId secondIdOld : originals)
        {
            // If both hedges are in alteredOriginals, we MUST NOT reconstruct the wiring
            if (alteredOriginals.count(firstIdOld) && alteredOriginals.count(secondIdOld))
                continue;
            // Check if also part of the result
            if (!original2new.count(secondIdOld))
                continue;
            // Get new uid
            UniqueId secondId = original2new[secondIdOld];
            // Skip if already handled
            if (handledEdges.count(secondId))
                continue;
            // Reconstruct wiring
            if (get(firstIdOld)->isPointingTo(secondIdOld))
                result.get(firstId)->to(secondId);
            if (get(secondIdOld)->isPointingTo(firstIdOld))
                result.get(secondId)->to(firstId);
            if (get(firstIdOld)->isPointingFrom(secondIdOld))
                result.get(firstId)->from(secondId);
            if (get(secondIdOld)->isPointingFrom(firstIdOld))
                result.get(secondId)->from(firstId);
        }
        // Remember that we handled all pairs (first,*)
        handledEdges.insert(firstId);
    }

    // B) Reconstruct wiring of the replacement graph
    handledEdges.clear();
    for (const UniqueId firstIdOld : replacements)
    {
        // Check if also part of the result
        if (!replacement2new.count(firstIdOld))
            continue;
        // Get new uid
        UniqueId firstId = replacement2new[firstIdOld];
        for (const UniqueId secondIdOld : replacements)
        {
            // Check if also part of the result
            if (!replacement2new.count(secondIdOld))
                continue;
            // Get new uid
            UniqueId secondId = replacement2new[secondIdOld];
            // Skip if already handled
            if (handledEdges.count(secondId))
                continue;
            // Reconstruct wiring
            if (rhs.get(firstIdOld)->isPointingTo(secondIdOld))
                result.get(firstId)->to(secondId);
            if (rhs.get(secondIdOld)->isPointingTo(firstIdOld))
                result.get(secondId)->to(firstId);
            if (rhs.get(firstIdOld)->isPointingFrom(secondIdOld))
                result.get(firstId)->from(secondId);
            if (rhs.get(secondIdOld)->isPointingFrom(firstIdOld))
                result.get(secondId)->from(firstId);
        }
        // Remember that we handled all pairs (first,*)
        handledEdges.insert(firstId);
    }

    return result;
}
