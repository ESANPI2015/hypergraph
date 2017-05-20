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

bool Hyperedge::pointTo(Hypergraph *graph, Hyperedge::Hyperedges otherIds)
{
    bool result = true;
    for (auto otherId : otherIds)
    {
        result &= pointTo(graph, otherId);
    }
    return result;
}

bool Hyperedge::pointTo(Hypergraph *graph, const unsigned id)
{
    // Check if we are part of the graph!!
    if (!isPartOf(graph))
        return false;

    // Create or find the other edge
    Hyperedge *edge = graph->get(id);
    if (!edge)
    {
        // Such a hyperedge does not exist, so we have to fail!!!
        return false;
    }

    // Make sure we are in the from list
    edge->_from.insert(_id);
    // ... and the edge registered in our to list
    _to.insert(edge->id());
    
    return true;
}

unsigned Hyperedge::id() const
{
    return _id;
}

std::string Hyperedge::label() const
{
    return _label;
}

unsigned Hyperedge::cardinality() const
{
    return _to.size();
}

bool Hyperedge::isPointingTo(const unsigned id)
{
    return _to.count(id) ? true : false;
}

bool Hyperedge::isPointedBy(const unsigned id)
{
    return _from.count(id) ? true : false;
}

Hyperedge::Hyperedges Hyperedge::pointedBy() const
{
    return _from;
}

Hyperedge::Hyperedges Hyperedge::pointingTo() const
{
    return _to;
}

bool Hyperedge::isPartOf(Hypergraph *graph)
{
    // TODO: Think about this ... should the ID be enough? Or is this the better way?
    return (graph->get(_id) == this) ? true : false;
}

Hyperedge::Hyperedges Hyperedge::pointedBy(Hypergraph *graph, const std::string& label)
{
    Hyperedges result;
    // Check if we are part of the graph!!
    if (!this->isPartOf(graph))
        return result;
    // Filter out nodes with some certain label
    for (auto edgeId : _from)
    {
        auto edge = graph->get(edgeId);
        // Filters by label if given. It suffices that the edge label contains the given one.
        if (label.empty() || (edge->label().find(label) != std::string::npos))
            result.insert(edge->id());
    }
    return result;
}

Hyperedge::Hyperedges Hyperedge::pointingTo(Hypergraph *graph, const std::string& label)
{
    Hyperedges result;
    // Check if we are part of the graph!!
    if (!isPartOf(graph))
        return result;
    // Filter out nodes with some certain label
    for (auto edgeId : _to)
    {
        auto edge = graph->get(edgeId);
        // Filters by label if given. It suffices that the edge label contains the given one.
        if (label.empty() || (edge->label().find(label) != std::string::npos))
            result.insert(edge->_id);
    }
    return result;
}

std::ostream& operator<< (std::ostream& stream, const Hyperedge* edge)
{
    stream << edge->id() << ":";
    stream << edge->label() << "[";
    auto otherIds = edge->pointingTo();
    for (auto otherId : otherIds)
    {
        stream << " " << otherId << " ";
    }
    stream << "]";
    return stream;
}

