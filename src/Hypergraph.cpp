#include "Hypergraph.hpp"
#include "Hyperedge.hpp"

Hypergraph::Hypergraph()
{
    _lastId = 1;
}

Hypergraph::Hypergraph(Hypergraph& A, Hypergraph& B)
{
    _lastId = 1;

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
        from(unite(A.get(id)->pointingFrom(), B.get(id)->pointingFrom()), id);
        to(id, unite(A.get(id)->pointingTo(), B.get(id)->pointingTo()));
    }
}

Hypergraph::~Hypergraph()
{
    // We hold no pointers so we do not need to do anything here
}

unsigned Hypergraph::getNextId()
{
    while (get(_lastId)) _lastId++;
    return _lastId++;
}

unsigned Hypergraph::create(const std::string& label)
{
    unsigned id = getNextId();
    _edges[id] = Hyperedge(id, label);
    return id;
}

unsigned Hypergraph::create(Hyperedges fromEdges, Hyperedges toEdges, const std::string& label)
{
    unsigned id = create(label);
    to(id, toEdges);
    from(fromEdges, id);
    return id;
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

Hypergraph::Hyperedges Hypergraph::find(const std::string& label,
                                        const std::string& lhs,
                                        const std::string& rhs
                                       )
{
    Hyperedges result;
    for (auto pair : _edges)
    {
        auto id = pair.first;
        auto edge = pair.second;
        // Filters by label if given. It suffices that the edge label contains the given one.
        if (label.empty() || (edge.label() == label))
            result.insert(id);
        // If edge does not match the label, skip it
        if (!label.empty() && (edge.label() != label))
            continue;
        // Find label in lhs
        if (!lhs.empty())
        {
            bool found = false;
            auto fromIds = edge.pointingFrom();
            for (auto fromId : fromIds)
            {
                auto other = get(fromId);
                if (other->label() == lhs)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                continue;
        }
        // Find label in rhs
        if (!rhs.empty())
        {
            bool found = false;
            auto toIds = edge.pointingTo();
            for (auto toId : toIds)
            {
                auto other = get(toId);
                if (other->label() == rhs)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                continue;
        }
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
