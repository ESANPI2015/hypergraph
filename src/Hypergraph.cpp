#include "Hypergraph.hpp"
#include "Hyperedge.hpp"

Hypergraph::Hypergraph()
{
    _lastId = 1;
}

Hypergraph::Hypergraph(Hypergraph& A, Hypergraph& B)
{
    _lastId = 1;

    // First pass: Creation
    std::map<unsigned,unsigned> old2newA;
    std::map<unsigned,unsigned> old2newB;
    for (auto idA : A.find())
    {
        // Create new node in C
        old2newA[idA] = create(A.get(idA)->label());
    }
    for (auto idB : B.find())
    {
        // Create new node in C
        old2newB[idB] = create(B.get(idB)->label());
    }

    // Second pass: Wiring
    for (auto idA : A.find())
    {
        auto newIdA = old2newA[idA];
        for (auto otherId : A.get(idA)->pointingTo())
        {
            auto newOtherId = old2newA[otherId];
            to(newIdA, newOtherId);
        }
        for (auto otherId : A.get(idA)->pointingFrom())
        {
            auto newOtherId = old2newA[otherId];
            from(newOtherId, newIdA);
        }
    }
    for (auto idB : B.find())
    {
        auto newIdB = old2newB[idB];
        for (auto otherId : B.get(idB)->pointingTo())
        {
            auto newOtherId = old2newB[otherId];
            to(newIdB, newOtherId);
        }
        for (auto otherId : B.get(idB)->pointingFrom())
        {
            auto newOtherId = old2newB[otherId];
            from(newOtherId, newIdB);
        }
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

Hyperedge::Hyperedges Hypergraph::find(const std::string& label) const
{
    Hyperedges result;
    for (auto pair : _edges)
    {
        auto id = pair.first;
        auto edge = pair.second;
        // Filters by label if given. It suffices that the edge label contains the given one.
        if (label.empty() || (edge.label() == label))
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

unsigned Hypergraph::unite(const unsigned idA, const unsigned idB)
{
    // The resulting hyperedge points to/from the union of the two link sets of A and B
    auto edgeA = get(idA);
    auto edgeB = get(idB);
    auto toedgesA = get(idA)->pointingTo();
    auto toedgesB = get(idB)->pointingTo();
    auto fromedgesA = get(idA)->pointingFrom();
    auto fromedgesB = get(idB)->pointingFrom();
    toedgesA.insert(toedgesB.begin(), toedgesB.end());
    fromedgesA.insert(fromedgesB.begin(), fromedgesB.end());

    return create(fromedgesA, toedgesA, edgeA->label() + "||" + edgeB->label());
}

unsigned Hypergraph::intersect(const unsigned idA, const unsigned idB)
{
    // The resulting hyperedge points to/from the edges which A and B points to/from
    auto edgeA = get(idA);
    auto edgeB = get(idB);
    auto toedgesA = get(idA)->pointingTo();
    auto toedgesB = get(idB)->pointingTo();
    auto fromedgesA = get(idA)->pointingFrom();
    auto fromedgesB = get(idB)->pointingFrom();
    Hyperedges toedgesC, fromedgesC;
    for (auto id : toedgesA)
    {
        if (toedgesB.count(id))
        {
            toedgesC.insert(id);
        }
    }
    for (auto id : fromedgesA)
    {
        if (fromedgesB.count(id))
        {
            fromedgesC.insert(id);
        }
    }

    return create(fromedgesC, toedgesC, edgeA->label() + "&&" + edgeB->label());
}

unsigned Hypergraph::subtract(const unsigned idA, const unsigned idB)
{
    // The resulting hyperedge points to/from the edges which A points to/from but B does not
    auto edgeA = get(idA);
    auto edgeB = get(idB);
    auto toedgesA = get(idA)->pointingTo();
    auto toedgesB = get(idB)->pointingTo();
    auto fromedgesA = get(idA)->pointingFrom();
    auto fromedgesB = get(idB)->pointingFrom();
    Hyperedges toedgesC, fromedgesC;
    for (auto id : toedgesA)
    {
        if (!toedgesB.count(id))
        {
            toedgesC.insert(id);
        }
    }
    for (auto id : fromedgesA)
    {
        if (!fromedgesB.count(id))
        {
            fromedgesC.insert(id);
        }
    }

    return create(fromedgesC, toedgesC, edgeA->label() + "/" + edgeB->label());
}

