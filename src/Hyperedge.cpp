#include "Hyperedge.hpp"
#include "Hypergraph.hpp"

#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <queue>
#include <stdexcept>

// PRIVATE CONSTRUCTORS
Hyperedge::Hyperedge(const UniqueId& id, const std::string& label)
: _id(id),
  _label(label)
{
}

// DESTRUCTORS
Hyperedge::~Hyperedge()
{
}

void Hyperedge::updateLabel(const std::string& label)
{
    _label = label;
}

void Hyperedge::from(const UniqueId& id)
{
    _from.insert(id);
}

void Hyperedge::to(const UniqueId& id)
{
    _to.insert(id);
}

const UniqueId& Hyperedge::id() const
{
    return _id;
}

const std::string& Hyperedge::label() const
{
    return _label;
}

const unsigned Hyperedge::indegree() const
{
    return _from.size();
}

const unsigned Hyperedge::outdegree() const
{
    return _to.size();
}

const bool Hyperedge::isPointingTo(const UniqueId& id) const
{
    return _to.count(id) ? true : false;
}

const bool Hyperedge::isPointingFrom(const UniqueId& id) const
{
    return _from.count(id) ? true : false;
}

const Hyperedges& Hyperedge::pointingFrom() const
{
    return _from;
}

const Hyperedges& Hyperedge::pointingTo() const
{
    return _to;
}

const bool Hyperedge::isPartOf(Hypergraph &graph) const
{
    // What does it mean to be part of a hypergraph?
    // At least it means, that <id,label> have to match, right?
    Hyperedge *ingraph(graph.get(id()));
    if (!ingraph)
        return false;
    if (ingraph->label() != label())
        return false;
    // TODO: Does it also mean, that all nodes in the _from and the _to set have to be part of the graph? I guess so
    return true;
}

std::ostream& operator<< (std::ostream& stream, const Hyperedge& edge)
{
    stream << "[";
    auto fromIds = edge.pointingFrom();
    for (auto otherId : fromIds)
    {
        stream << " " << otherId << " ";
    }
    stream << "] ";
    stream << edge.id() << ":";
    stream << edge.label() << " [";
    auto toIds = edge.pointingTo();
    for (auto otherId : toIds)
    {
        stream << " " << otherId << " ";
    }
    stream << "]";
    return stream;
}

Hyperedges unite(const Hyperedges& a, const Hyperedges& b)
{
    Hyperedges result(a);
    result.insert(b.begin(), b.end());
    return result;
}

Hyperedges intersect(const Hyperedges& a, const Hyperedges& b)
{
    Hyperedges result;
    for (auto id : a)
    {
        if (b.count(id))
            result.insert(id);
    }
    return result;
}

Hyperedges subtract(const Hyperedges& a, const Hyperedges& b)
{
    Hyperedges result;
    for (auto id : a)
    {
        if (!b.count(id))
            result.insert(id);
    }
    return result;
}

std::ostream& operator<< (std::ostream& os , const Hyperedges& val)
{
    os << "{ ";
    for (auto id : val)
    {
        os << id << " ";
    }
    os << "} ";
    return os;
}
