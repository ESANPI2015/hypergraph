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

Mapping Hypergraph::match(Hypergraph& other, const std::vector< Mapping >& previousMatches)
{
    // This algorithm is according to Ullmann
    // and has been implemented following "An In-depth Comparison of Subgraph Isomorphism Algorithms in Graph Databases"
    // First step: For each vertex in subgraph, we find other suitable candidates
    Mapping currentMapping;
    std::map< UniqueId, Hyperedges > candidateIds;
    Hyperedges otherIds = other.find();
    for (UniqueId otherId : otherIds)
    {
        // NOTE: Do not prevent the trivial case here!!!
        candidateIds[otherId] = find(other.get(otherId)->label());
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
            // Check if currentMapping is different from all previous matches in at least one association
            bool newMatch = true;
            for (Mapping prev : previousMatches)
            {
                if (equal(currentMapping, prev))
                {
                    newMatch = false;
                    break;
                }
            }
            // If it is a true new match, we can proceed
            if (!newMatch)
                continue; // but otherwise we continue
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
                mappings.push(newMapping);
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

Mapping Hypergraph::rewrite(Hypergraph& other, const Mapping& replacements)
{
    Mapping result;
    // Apply explicit replacements
    for (const auto &pair : replacements)
    {
        // Get some important UIDs
        UniqueId originalId(pair.first);
        UniqueId replacementId(pair.second);

        // 0 -> X case: create a new node and store its UID in originalId
        if (originalId.empty() && !replacementId.empty())
        {
            unsigned occurrence = 1;
            originalId = replacementId;
            while (create(originalId, other.get(replacementId)->label()).empty())
                originalId = replacementId + std::to_string(occurrence++);
        } 
        // Y -> 0 case: A former node in DG has to be destroyed
        if (!originalId.empty() && replacementId.empty())
        {
            destroy(originalId);
            continue;
        }
        // Y -> X case: Update label (if given)
        if (!originalId.empty() && !replacementId.empty())
        {
            // If the replacement hedge provides a label, we change the label of the original
            std::string replacementLabel(other.get(replacementId)->label());
            if (!replacementLabel.empty())
            {
                get(originalId)->updateLabel(replacementLabel);
            }
        }
        result[originalId] = replacementId;
    }

    // TODO: Can we really do this invert?
    Mapping resultInv(invert(result));
    // Wire the nodes
    for (const auto& pair : result)
    {
        // Get UIDs
        UniqueId originalId(pair.first);
        UniqueId replacementId(pair.second);

        // If one of the UIDs is empty, just continue
        if (originalId.empty())
            continue;
        if (replacementId.empty())
            continue;

        // Get some needed connection info
        Hyperedges originalPointsTo = to(Hyperedges{originalId});
        Hyperedges replacementPointsTo = other.to(Hyperedges{replacementId});
        Hyperedges originalPointsFrom = from(Hyperedges{originalId});
        Hyperedges replacementPointsFrom = other.from(Hyperedges{replacementId});

        // Make sure that links from/to originals are replaced by links from/to correspnding replacements
        // Check (original -> originalOther) -> (replacement -> replacementOther)
        for (UniqueId originalOtherId : originalPointsTo)
        {
            // Find the corresponding replacement for originalOtherId
            if (!result.count(originalOtherId))
                continue;
            UniqueId replacementOtherId = result[originalOtherId];
            // Now we have to check if (originalId -> originalOtherId) -> (replacementId -> replacementOtherId) is true
            if (replacementPointsTo.count(replacementOtherId))
                continue;
            // If not, we have to remove (originalId -> originalOtherId)
            get(originalId)->_to.erase(originalOtherId);
        }
        // Check (original -> originalOther) <- (replacement -> replacementOther)
        for (UniqueId replacementOtherId : replacementPointsTo)
        {
            // Find the corresponding original for replacementOtherId
            if (!resultInv.count(replacementOtherId))
                continue;
            UniqueId originalOtherId = resultInv[replacementOtherId];
            // Now we have to check if (originalId -> originalOtherId) <- (replacementId -> replacementOtherId) is true
            if (originalOtherId.empty() || originalPointsTo.count(originalOtherId))
                continue;
            // If not, we have to add (originalId -> originalOtherId)
            get(originalId)->_to.insert(originalOtherId);
        }
        // We did it for the TO links, now we also have to do it for the FROM links
        // Check (original -> originalOther) -> (replacement -> replacementOther)
        for (UniqueId originalOtherId : originalPointsFrom)
        {
            // Find the corresponding replacement for originalOtherId
            if (!result.count(originalOtherId))
                continue;
            UniqueId replacementOtherId = result[originalOtherId];
            // Now we have to check if (originalId -> originalOtherId) -> (replacementId -> replacementOtherId) is true
            if (replacementPointsFrom.count(replacementOtherId))
                continue;
            // If not, we have to remove (originalId -> originalOtherId)
            get(originalId)->_from.erase(originalOtherId);
        }
        // Check (original -> originalOther) <- (replacement -> replacementOther)
        for (UniqueId replacementOtherId : replacementPointsFrom)
        {
            // Find the corresponding original for replacementOtherId
            if (!resultInv.count(replacementOtherId))
                continue;
            UniqueId originalOtherId = resultInv[replacementOtherId];
            // Now we have to check if (originalId -> originalOtherId) <- (replacementId -> replacementOtherId) is true
            if (originalOtherId.empty() || originalPointsFrom.count(originalOtherId))
                continue;
            // If not, we have to add (originalId -> originalOtherId)
            get(originalId)->_from.insert(originalOtherId);
        }
    }
    return result;
}

