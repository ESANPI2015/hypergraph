#include "Hypergraph.hpp"
#include "Hyperedge.hpp"

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
    else if (neu->label() == label)
    {
        // If a hyperedge with the same id and label exists, it is assumed to have been created and counts as success
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
