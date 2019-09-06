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

void Hyperedge::pointsFrom(const UniqueId& id)
{
    _from.push_back(id);
}

void Hyperedge::pointsTo(const UniqueId& id)
{
    _to.push_back(id);
}

const UniqueId& Hyperedge::id() const
{
    return _id;
}

const std::string& Hyperedge::label() const
{
    return _label;
}

unsigned Hyperedge::indegree() const
{
    return _from.size();
}

unsigned Hyperedge::outdegree() const
{
    return _to.size();
}

bool Hyperedge::isPointingTo(const UniqueId& id) const
{
    return std::find(_to.begin(), _to.end(), id) != _to.end() ? true : false;
}

bool Hyperedge::isPointingFrom(const UniqueId& id) const
{
    return std::find(_from.begin(), _from.end(), id) != _from.end() ? true : false;
}

const Hyperedges& Hyperedge::pointingFrom() const
{
    return _from;
}

const Hyperedges& Hyperedge::pointingTo() const
{
    return _to;
}

bool Hyperedge::isPartOf(Hypergraph &graph) const
{
    // What does it mean to be part of a hypergraph?
    // At least it means, that <id,label> have to match, right?
    const Hyperedge& ingraph(graph.access(id()));
    if (ingraph.id() == Hypergraph::Zero)
        return false;
    if (ingraph.label() != label())
        return false;
    // TODO: Does it also mean, that all nodes in the _from and the _to set have to be part of the graph? I guess so
    return true;
}

std::ostream& operator<< (std::ostream& stream, const Hyperedge& edge)
{
    stream << "[";
    const Hyperedges& fromIds(edge.pointingFrom());
    for (const auto& otherId : fromIds)
    {
        stream << " " << otherId << " ";
    }
    stream << "] ";
    stream << edge.id() << ":";
    stream << edge.label() << " [";
    const Hyperedges& toIds(edge.pointingTo());
    for (const auto& otherId : toIds)
    {
        stream << " " << otherId << " ";
    }
    stream << "]";
    return stream;
}

Hyperedges unite(const Hyperedges& a, const Hyperedges& b)
{
    Hyperedges result(a);
    for (const auto& id : b)
    {
        if (std::find(a.begin(), a.end(), id) == a.end())
            result.push_back(id);
    }
    return result;
}

Hyperedges intersect(const Hyperedges& a, const Hyperedges& b)
{
    Hyperedges result;
    for (const auto& id : a)
    {
        if (std::find(b.begin(), b.end(), id) != b.end())
            result.push_back(id);
    }
    return result;
}

Hyperedges subtract(const Hyperedges& a, const Hyperedges& b)
{
    Hyperedges result;
    for (const auto& id : a)
    {
        if (std::find(b.begin(), b.end(), id) == b.end())
            result.push_back(id);
    }
    return result;
}

std::ostream& operator<< (std::ostream& os , const Hyperedges& val)
{
    os << "{ ";
    for (const auto& id : val)
    {
        os << id << " ";
    }
    os << "} ";
    return os;
}
