#include "Hypergraph.hpp"
#include "Hyperedge.hpp"

Hypergraph::Hypergraph()
{
    _lastId = 1;
}

Hypergraph::~Hypergraph()
{
    // Destroy only those edges which have been created by factory (all!)
    auto localCopy = _edges;

    // finally destroy
    for (auto edgeIt : localCopy)
    {
        auto edge = edgeIt.second;
        delete edge;
    }

    _edges.clear();
}

unsigned Hypergraph::getNextId()
{
    while (get(_lastId)) _lastId++;
    return _lastId++;
}

unsigned Hypergraph::create(const std::string& label)
{
    Hyperedge* neu = new Hyperedge(getNextId(), label);
    _edges[neu->id()] = neu;
    return neu->id();
}

unsigned Hypergraph::create(Hyperedges edges, const std::string& label)
{
    Hyperedge* neu = get(Hypergraph::create(label));
    for (auto edgeId : edges)
    {
        fromTo(neu->id(), edgeId);
    }
    return neu->id();
}

bool Hypergraph::create(const unsigned id, const std::string& label)
{
    Hyperedge* neu = get(id);
    if (!neu)
    {
        // Create a new hyperedge
        // Give it the desired id
        neu = new Hyperedge(id, label);
        _edges[id] = neu;
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

    // delete permanently
    delete edge;
}

void Hypergraph::disconnect(const unsigned id)
{
    auto edge = get(id);
    if (!edge)
        return;

    // others -> edge
    for (auto edgeId : edge->pointedBy())
    {
        auto other = get(edgeId);
        if (other && other->isPointingTo(id))
        {
            other->_to.erase(id);
        }
    }
    // edge -> others
    for (auto edgeId : edge->pointingTo())
    {
        auto other = get(edgeId);
        if (other && other->isPointedBy(id))
        {
            other->_from.erase(id);
        }
    }
}

Hyperedge* Hypergraph::get(const unsigned id)
{
    if (_edges.count(id))
    {
        return _edges[id];
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
        if (label.empty() || (edge->label() == label))
            result.insert(id);
    }
    return result;
}


bool Hypergraph::fromTo(const unsigned srcId, const unsigned destId)
{
    auto srcEdge = get(srcId);
    if (!srcEdge)
        return false;
    return srcEdge->pointTo(this, destId);
}

unsigned Hypergraph::unite(const unsigned idA, const unsigned idB)
{
    // We will create a Hyperedge which will contain
    // x which are part of either A->edges() or B->edges() or both
    auto edgeA = get(idA);
    auto edgeB = get(idB);
    auto edgesA = get(idA)->pointingTo();
    auto edgesB = get(idB)->pointingTo();
    edgesA.insert(edgesB.begin(), edgesB.end());
    return create(edgesA, edgeA->label() + "||" + edgeB->label());
}

unsigned Hypergraph::intersect(const unsigned idA, const unsigned idB)
{
    // We will create a Hyperedge which will contain
    // x which are part of both A->edges() and B->edges()
    auto edgeA = get(idA);
    auto edgeB = get(idB);
    auto edgesA = get(idA)->pointingTo();
    auto edgesB = get(idB)->pointingTo();
    Hyperedges edgesC;
    for (auto id : edgesA)
    {
        if (edgesB.count(id))
        {
            edgesC.insert(id);
        }
    }
    return create(edgesC, edgeA->label() + "&&" + edgeB->label());
}

unsigned Hypergraph::subtract(const unsigned idA, const unsigned idB)
{
    // We will create a hyperedge with
    // only those x which are part of A->edges() but not part of B->edges()
    auto edgeA = get(idA);
    auto edgeB = get(idB);
    auto edgesA = get(idA)->pointingTo();
    auto edgesB = get(idB)->pointingTo();
    Hyperedges edgesC;
    for (auto id : edgesA)
    {
        if (!edgesB.count(id))
        {
            edgesC.insert(id);
        }
    }
    return create(edgesC, edgeA->label() + "/" + edgeB->label());
}

Hypergraph* Hypergraph::Union(Hypergraph* A, Hypergraph *B)
{
    // Construct a new graph out of two others
    Hypergraph *result = new Hypergraph;

    // First pass: Creation
    std::map<unsigned,unsigned> old2newA;
    std::map<unsigned,unsigned> old2newB;
    for (auto idA : A->find())
    {
        // Create new node in C
        old2newA[idA] = result->create(A->get(idA)->label());
    }
    for (auto idB : B->find())
    {
        // Create new node in C
        old2newB[idB] = result->create(B->get(idB)->label());
    }

    // Second pass: Wiring
    for (auto idA : A->find())
    {
        auto newIdA = old2newA[idA];
        for (auto otherId : A->get(idA)->pointingTo())
        {
            auto newOtherId = old2newA[otherId];
            result->get(newIdA)->pointTo(result, newOtherId);
        }
    }
    for (auto idB : B->find())
    {
        auto newIdB = old2newB[idB];
        for (auto otherId : B->get(idB)->pointingTo())
        {
            auto newOtherId = old2newB[otherId];
            result->get(newIdB)->pointTo(result, newOtherId);
        }
    }
    return result;
}
