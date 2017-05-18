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

unsigned Hypergraph::create(const std::string& label)
{
    Hyperedge* neu = new Hyperedge(label);
    while (get(_lastId)) _lastId++;
    neu->_id = _lastId;
    _edges[_lastId++] = neu;
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
        neu = new Hyperedge(label);
        // Give it the desired id
        neu->_id = id;
        _edges[id] = neu;
        return true;
    }
    return false;
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

Hyperedge::Hyperedges Hypergraph::find(const std::string& label)
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

//Hyperedge* Hyperedge::labelContains(const std::string& str)
//{
//    return Hyperedge::create(_traversal(
//        [&](Hyperedge *x){ return (str.empty() || (x->label().find(str) != std::string::npos)) ? true : false; },
//        [](Hyperedge *x, Hyperedge *y){return true;},
//        BOTH),
//        "labelContains(" + str + ")"
//    );
//}
//
//Hyperedge* Hyperedge::labelPartOf(const std::string& str)
//{
//    return Hyperedge::create(_traversal(
//        [&](Hyperedge *x){ return (str.empty() || (str.find(x->label()) != std::string::npos)) ? true : false; },
//        [](Hyperedge *x, Hyperedge *y){return true;},
//        BOTH),
//        "labelPartOf(" + str + ")"
//    );
//}
//
//Hyperedge* Hyperedge::cardinalityLessThanOrEqual(const unsigned cardinality)
//{
//    std::stringstream ss;
//    ss << "cardinalityLessThanOrEqual(" << cardinality << ")";
//    return Hyperedge::create(_traversal(
//        [&](Hyperedge *x){ return (x->cardinality() <= cardinality)? true : false; },
//        [](Hyperedge *x, Hyperedge *y){return true;},
//        BOTH),
//        ss.str()
//    );
//}
//
//Hyperedge* Hyperedge::cardinalityGreaterThan(const unsigned cardinality)
//{
//    std::stringstream ss;
//    ss << "cardinalityGreaterThan(" << cardinality << ")";
//    return Hyperedge::create(_traversal(
//        [&](Hyperedge *x){ return (x->cardinality() > cardinality)? true : false; },
//        [](Hyperedge *x, Hyperedge *y){return true;},
//        BOTH),
//        ss.str()
//    );
//}
//
//Hyperedge* Hyperedge::successors()
//{
//    return Hyperedge::create(_traversal(
//        [&](Hyperedge *x){ return x->id() != id() ? true : false; },
//        [](Hyperedge *x, Hyperedge *y){return true;},
//        DOWN),
//        "successors(" + _label + ")"
//    );
//}
//
//Hyperedge* Hyperedge::predecessors()
//{
//    return Hyperedge::create(_traversal(
//        [&](Hyperedge *x){ return x->id() != id() ? true : false; },
//        [](Hyperedge *x, Hyperedge *y){return true;},
//        UP),
//        "predecessors(" + _label + ")"
//    );
//}

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
