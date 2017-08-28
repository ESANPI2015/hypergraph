#include "Hyperedge.hpp"
#include "Hypergraph.hpp"

#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <queue>
#include <stdexcept>

// PRIVATE CONSTRUCTORS
Hyperedge::Hyperedge(const unsigned id, const std::string& label)
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

void Hyperedge::from(const unsigned id)
{
    _from.insert(id);
}

void Hyperedge::to(const unsigned id)
{
    _to.insert(id);
}

unsigned Hyperedge::id() const
{
    return _id;
}

std::string Hyperedge::label() const
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

bool Hyperedge::isPointingTo(const unsigned id)
{
    return _to.count(id) ? true : false;
}

bool Hyperedge::isPointingFrom(const unsigned id)
{
    return _from.count(id) ? true : false;
}

Hyperedge::Hyperedges Hyperedge::pointingFrom() const
{
    return _from;
}

Hyperedge::Hyperedges Hyperedge::pointingTo() const
{
    return _to;
}

bool Hyperedge::isPartOf(Hypergraph &graph)
{
    // What does it mean to be part of a hypergraph?
    // At least it means, that <id,label> have to match, right?
    Hyperedge *ingraph = graph.get(id());
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

