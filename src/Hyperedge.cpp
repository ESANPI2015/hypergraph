#include "Hyperedge.hpp"
#include "Hypergraph.hpp"

#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <queue>
#include <stdexcept>

// PRIVATE CONSTRUCTORS
Hyperedge::Hyperedge(const UniqueId& id, const std::string& label, const Properties& props)
: _id(id),
  _properties(props)
{
    property("label",label);
}

// DESTRUCTORS
Hyperedge::~Hyperedge()
{
}

const Properties& Hyperedge::properties() const 
{
    return _properties;
}

const std::string& Hyperedge::property(const std::string& key) const
{
    return _properties.at(key);
}

bool Hyperedge::hasProperty(const std::string& key) const
{
    if (_properties.find(key) != _properties.end())
        return true;
    return false;
}

void Hyperedge::property(const std::string& key, const std::string& val)
{
    _properties[key] = val;
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

std::ostream& operator<< (std::ostream& stream, const Hyperedge& edge)
{
    stream << edge.pointingFrom();
    stream << " " << edge.id() << ":";
    stream << edge.label() << " ";
    stream << edge.pointingTo();
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
    os << "[";
    for (const auto& id : val)
    {
        os << " " << id;
    }
    os << " ]";
    return os;
}
