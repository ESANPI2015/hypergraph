#include "Hypergraph.hpp"
#include "Hyperedge.hpp"

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

bool Hypergraph::create(const unsigned id, const std::string& label)
{
    Hyperedge* neu = get(id);
    if (!neu)
    {
        // Create a new hyperedge
        // Give it the desired id
        _edges[id] = Hyperedge(id, label);
        return true;
    }
    return false;
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

Hypergraph::Hyperedges Hypergraph::find(const std::string& label)
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


bool Hypergraph::to(const unsigned srcId, const unsigned destId)
{
    auto srcEdge = get(srcId);
    auto destEdge = get(destId);
    if (!srcEdge || !destEdge)
        return false;
    srcEdge->to(destId);
    return true;
}

bool Hypergraph::to(const unsigned srcId, const Hyperedges otherIds)
{
    auto srcEdge = get(srcId);
    if (!srcEdge)
        return false;
    for (auto otherId : otherIds)
    {
        // Check if other is part of this graph as well
        auto other = get(otherId);
        if (!other)
            return false;
        srcEdge->to(otherId);
    }
    return true;
}

bool Hypergraph::from(const unsigned srcId, const unsigned destId)
{
    auto srcEdge = get(srcId);
    auto destEdge = get(destId);
    if (!srcEdge || !destEdge)
        return false;
    destEdge->from(srcId);
    return true;
}

bool Hypergraph::from(const Hyperedges otherIds, const unsigned destId)
{
    auto destEdge = get(destId);
    if (!destEdge)
        return false;
    for (auto otherId : otherIds)
    {
        // Check if other is part of this graph as well
        auto other = get(otherId);
        if (!other)
            return false;
        destEdge->from(otherId);
    }
    return true;
}

Hypergraph::Hyperedges Hypergraph::from(const unsigned id, const std::string& label)
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

Hypergraph::Hyperedges Hypergraph::to(const unsigned id, const std::string& label)
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

Hypergraph::Hyperedges Hypergraph::from(const Hyperedges& ids, const std::string& label)
{
    Hyperedges result;
    for (auto id : ids)
    {
        auto fromIds = from(id, label);
        result.insert(fromIds.begin(), fromIds.end());
    }
    return result;
}

Hypergraph::Hyperedges Hypergraph::to(const Hyperedges& ids, const std::string& label)
{
    Hyperedges result;
    for (auto id : ids)
    {
        auto toIds = to(id, label);
        result.insert(toIds.begin(), toIds.end());
    }
    return result;
}

Hypergraph::Hyperedges Hypergraph::unite(const Hyperedges& edgesA, const Hyperedges& edgesB)
{
    Hyperedges result(edgesA);
    result.insert(edgesB.begin(), edgesB.end());
    return result;
}

Hypergraph::Hyperedges Hypergraph::intersect(const Hyperedges& edgesA, const Hyperedges& edgesB)
{
    Hyperedges edgesC;
    for (auto id : edgesA)
    {
        if (edgesB.count(id))
        {
            edgesC.insert(id);
        }
    }
    return edgesC;
}

Hypergraph::Hyperedges Hypergraph::subtract(const Hyperedges& edgesA, const Hyperedges& edgesB)
{
    Hyperedges edgesC;
    for (auto id : edgesA)
    {
        if (!edgesB.count(id))
        {
            edgesC.insert(id);
        }
    }
    return edgesC;
}

Hypergraph::Hyperedges Hypergraph::prevNeighboursOf(const unsigned id, const std::string& label)
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

Hypergraph::Hyperedges Hypergraph::prevNeighboursOf(const Hyperedges& ids, const std::string& label)
{
    Hyperedges result;
    for (unsigned id : ids)
    {
        Hyperedges n = prevNeighboursOf(id, label);
        result.insert(n.begin(), n.end());
    }
    return result;
}

Hypergraph::Hyperedges Hypergraph::nextNeighboursOf(const unsigned id, const std::string& label)
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

Hypergraph::Hyperedges Hypergraph::nextNeighboursOf(const Hyperedges& ids, const std::string& label)
{
    Hyperedges result;
    for (unsigned id : ids)
    {
        Hyperedges n = nextNeighboursOf(id, label);
        result.insert(n.begin(), n.end());
    }
    return result;
}

Hypergraph::Hyperedges Hypergraph::allNeighboursOf(const unsigned id, const std::string& label)
{
    Hyperedges result;
    result = unite(prevNeighboursOf(id,label), nextNeighboursOf(id,label));
    return result;
}

Hypergraph::Hyperedges Hypergraph::allNeighboursOf(const Hyperedges& ids, const std::string& label)
{
    Hyperedges result;
    for (unsigned id : ids)
    {
        Hyperedges n = allNeighboursOf(id, label);
        result.insert(n.begin(), n.end());
    }
    return result;
}

Hypergraph::Hyperedges Hypergraph::match(Hypergraph& other)
{
    // This algorithm is according to Ullmann
    // and has been implemented following "An In-depth Comparison of Subgraph Isomorphism Algorithms in Graph Databases"
    Hyperedges result;
    typedef std::pair<unsigned,unsigned> HedgePair;
    typedef std::set< HedgePair > HedgePairSet;
    
    // First step: For each vertex in other, we find suitable candidates in this graph
    Hyperedges otherIds = other.find();
    std::map< unsigned, Hyperedges > candidateIds;
    for (unsigned otherId : otherIds)
    {
        candidateIds[otherId] = find(other.get(otherId)->label());
        if (!candidateIds[otherId].size())
            return result;
    }

    // Second step: Find possible mapping(s)
    // We need a stack of mappings to prevent recursion
    HedgePairSet currentMapping;
    std::stack< HedgePairSet > mappings;
    mappings.push(HedgePairSet());
    while (!mappings.empty())
    {
        // Get top of stack
        currentMapping = mappings.top();
        mappings.pop();

        // Check if we can stop the search
        if (currentMapping.size() == otherIds.size())
            break;

        // Otherwise search for a possible new mapping and proceed
        // TODO: This should be optimized
        unsigned unmappedId = 0;
        for (unsigned otherId : otherIds)
        {
            bool unmapped = true;
            unmappedId = otherId;
            for (HedgePair pair : currentMapping)
            {
                if (pair.first == otherId)
                {
                    unmapped = false;
                    break;
                } 
            }
            if (unmapped)
                break;
        }

        // Found unmapped hedge
        Hyperedges candidates = candidateIds[unmappedId];
        Hyperedges neighbourIds = other.allNeighboursOf(unmappedId);
        for (unsigned candidateId : candidates)
        {
            Hyperedges candidateNeighbours = allNeighboursOf(candidateId);
            // Ignore all candidates whose neighbourhood is smaller
            if (candidateNeighbours.size() < neighbourIds.size())
                continue;
            bool foundMatch = true;
            for (unsigned neighbourId : neighbourIds)
            {
                // Check if neighbour is already matched
                bool matched = false;
                HedgePair matchedPair;
                for (HedgePair pair : currentMapping)
                {
                    matchedPair = pair;
                    if (pair.first == neighbourId)
                    {
                        matched = true;
                        break;
                    }
                }
                // If it is matched, check if there is an edge between pair.second and candidateId
                if (matched && !candidateNeighbours.count(matchedPair.second))
                {
                    foundMatch = false;
                }
            }
            if (foundMatch)
            {
                // Insert match
                HedgePair newMatch(unmappedId, candidateId);
                HedgePairSet newMapping(currentMapping);
                newMapping.insert(newMatch);
                mappings.push(newMapping);
            }
        }
    }

    // Here we should check if we have a mapping or not and convert it
    if (currentMapping.size() == otherIds.size())
    {
        //std::cout << "Match found\n";
        for (HedgePair pair : currentMapping)
        {
            //std::cout << pair.first << " -> " << pair.second << "\n";
            result.insert(pair.second);
        }
    }
    return result;
}

