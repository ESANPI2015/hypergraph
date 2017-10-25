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

Mapping Hypergraph::match(const Hyperedges& otherIds, const std::vector< Mapping >& previousMatches)
{
    // This algorithm is according to Ullmann
    // and has been implemented following "An In-depth Comparison of Subgraph Isomorphism Algorithms in Graph Databases"
    // First step: For each vertex in subgraph, we find other suitable candidates
    Mapping currentMapping;
    std::map< UniqueId, Hyperedges > candidateIds;
    for (UniqueId otherId : otherIds)
    {
        // NOTE: Do not prevent the trivial case here!!!
        candidateIds[otherId] = find(get(otherId)->label());
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
            Hyperedges templatePointsTo(intersect(to(Hyperedges{pair.first}), otherIds));
            Hyperedges templatePointsFrom(intersect(from(Hyperedges{pair.first}), otherIds));
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
        Hyperedges nextNeighbourIds = intersect(to(Hyperedges{unmappedId}), otherIds); // Check ONLY the neighbourhood INSIDE the subgraph
        Hyperedges prevNeighbourIds = intersect(from(Hyperedges{unmappedId}), otherIds);
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

Mapping Hypergraph::rewrite(Mapping& matched, Mapping& replacements)
{
    // matched contains a mapping from a model subgraph to some ismorphism of it in the overall graph
    // replacements contains a mapping from the same model subgraph to another (sub)graph
    Mapping result;
    // Since we know that matched is a one-to-one mapping, we can invert it
    Mapping matchedInv(invert(matched));
    // Do this also for replacements
    Mapping replacementsInv(invert(replacements));

    // Cycle through all entries of matched
    for (const auto &pair : matched)
    {
        UniqueId modelId = pair.first;
        UniqueId originalId = pair.second;
        UniqueId replacementId;
        if (replacements.count(modelId))
            replacementId = replacements[modelId];

        // Register the change
        result[originalId] = replacementId;

        // If the replacementId is not given/is zero, we delete the original hyperedge and proceed
        if (replacementId.empty())
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
        Hyperedges originalPointsTo = to(Hyperedges{originalId});
        Hyperedges replacementPointsTo = to(Hyperedges{replacementId});
        // Check (original -> originalOther) -> (replacement -> replacementOther)
        for (UniqueId originalOtherId : originalPointsTo)
        {
            // Check if originalOther has also been matched
            if (!matchedInv.count(originalOtherId))
                continue;
            UniqueId matchedOtherId = matchedInv[originalOtherId];
            // Check if there is a replacement for originalOther
            if (!replacements.count(matchedOtherId))
                continue;
            UniqueId replacementOtherId = replacements[matchedOtherId];
            // Now we have to check if (originalId -> originalOtherId) -> (replacementId -> replacementOtherId) is true
            if (replacementPointsTo.count(replacementOtherId))
                continue;
            // If not, we have to remove (originalId -> originalOtherId)
            get(originalId)->_to.erase(originalOtherId);
        }
        // Check (original -> originalOther) <- (replacement -> replacementOther)
        for (UniqueId replacementOtherId : replacementPointsTo)
        {
            // Get the matched id
            if (!replacementsInv.count(replacementOtherId))
            {
                // TODO: If this node can not be found, we have to create it and let original point to it!
                continue;
            }
            UniqueId matchedOtherId = replacementsInv[replacementOtherId];
            // Check if there is a match
            if (!matched.count(matchedOtherId))
            {
                // TODO: It would be a serious error of the matching algorithm if we end up here!
                continue;
            }
            UniqueId originalOtherId = matched[matchedOtherId];
            // Now we have to check if (originalId -> originalOtherId) <- (replacementId -> replacementOtherId) is true
            if (originalPointsTo.count(originalOtherId))
                continue;
            // If not, we have to add (originalId -> originalOtherId)
            get(originalId)->_to.insert(originalOtherId);
        }
        // Do the same with the from set
        Hyperedges originalPointsFrom = from(Hyperedges{originalId});
        Hyperedges replacementPointsFrom = from(Hyperedges{replacementId});
        // Check (originalOther <- original) -> (replacementOther <- replacement)
        for (UniqueId originalOtherId : originalPointsFrom)
        {
            // Check if originalOther has also been matched
            if (!matchedInv.count(originalOtherId))
                continue;
            UniqueId matchedOtherId = matchedInv[originalOtherId];
            // Check if there is a replacement for originalOther
            if (!replacements.count(matchedOtherId))
                continue;
            UniqueId replacementOtherId = replacements[matchedOtherId];
            // Now we have to check if (originalOtherId <- originalId) -> (replacementOtherId <- replacementId) is true
            if (replacementPointsFrom.count(replacementOtherId))
                continue;
            // If not, we have to remove (originalOtherId <- originalId)
            get(originalId)->_from.erase(originalOtherId);
        }
        // Check (originalOther <- original) <- (replacementOther <- replacement)
        for (UniqueId replacementOtherId : replacementPointsFrom)
        {
            // Get the matched id
            if (!replacementsInv.count(replacementOtherId))
            {
                // TODO: If this node can not be found, we have to create it and let original point from it!
                continue;
            }
            UniqueId matchedOtherId = replacementsInv[replacementOtherId];
            // Check if there is a match
            if (!matched.count(matchedOtherId))
            {
                // TODO: It would be a serious error of the matching algorithm if we end up here!
                continue;
            }
            UniqueId originalOtherId = matched[matchedOtherId];
            // Now we have to check if (originalOtherId <- originalId) <- (replacementOtherId <- replacementId) is true
            if (originalPointsFrom.count(originalOtherId))
                continue;
            // If not, we have to add (originalOtherId <- originalId)
            get(originalId)->_from.insert(originalOtherId);
        }
    }
    return result;
}
