#include "Hypergraph.hpp"

#include <iostream>

Hypergraph::Hypergraph()
{
}

Hypergraph::Hypergraph(const Hypergraph& other)
{
    importFrom(other);
}

Hypergraph::Hypergraph(const Hypergraph& A, const Hypergraph& B)
{
    importFrom(A);
    importFrom(B);
}

Hypergraph::~Hypergraph()
{
    // We hold no pointers so we do not need to do anything here
}

void Hypergraph::importFrom(const Hypergraph& other)
{
    // When we construct a graph from other, we have to repopulate the cache by rewiring
    // First pass: Clone hedges
    for (const UniqueId& id : other.find())
    {
        create(id, other._edges.at(id).label());
    }

    // Second pass: Rewire (see from and to methods which populate the cache(s))
    for (const UniqueId id : find())
    {
        if (!other._edges.count(id))
            continue;
        // Wire only those things which have not yet been wired before (otherwise we get arity changes)
        Hyperedges newFromUids(subtract(other._edges.at(id).pointingFrom(), _edges.at(id).pointingFrom()));
        Hyperedges newToUids(subtract(other._edges.at(id).pointingTo(), _edges.at(id).pointingTo()));
        from(newFromUids, Hyperedges{id});
        to(Hyperedges{id}, newToUids);
    }
}

Hyperedges Hypergraph::create(const UniqueId id, const std::string& label)
{
    Hyperedge* neu = get(id);
    if (!neu)
    {
        // Create a new hyperedge
        // Give it the desired id
        _edges[id] = Hyperedge(id, label);
        return Hyperedges{id};
    }
    return Hyperedges();
}

void Hypergraph::destroy(const UniqueId id)
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

void Hypergraph::disconnect(const UniqueId id)
{
    // We have to find all edges referring to us!
    // TODO: Can we use the cache here?
    Hyperedges all = find();
    for (auto otherId : all)
    {
        auto other = get(otherId);
        if (other->isPointingTo(id))
        {
            // remove id from _to set
            std::remove(other->_to.begin(), other->_to.end(), id);
        }
        if (other->isPointingFrom(id))
        {
            // remove id from _from set
            std::remove(other->_from.begin(), other->_from.end(), id);
        }
    }
}

const Hyperedge& Hypergraph::read(const UniqueId id) const
{
    return _edges.at(id);
}

Hyperedge* Hypergraph::get(const UniqueId id)
{
    if (_edges.count(id))
    {
        return &_edges[id];
    } else {
        return NULL;
    }
}

Hyperedges Hypergraph::find(const std::string& label) const
{
    Hyperedges result;
    for (auto pair : _edges)
    {
        auto id = pair.first;
        auto edge = pair.second;
        // If edge does not match the label, skip it
        if (!label.empty() && (edge.label() != label))
            continue;
        result.push_back(id);
    }
    return result;
}


Hyperedges Hypergraph::from(const Hyperedges& otherIds, const Hyperedges& destIds)
{
    Hyperedges result;
    for (const UniqueId& destId : destIds)
    {
        Hyperedge* destEdge = get(destId);
        if (!destEdge)
            continue;
        for (const UniqueId& otherId : otherIds)
        {
            // Check if other is part of this graph as well
            Hyperedge* other = get(otherId);
            if (!other)
                continue;
            destEdge->from(otherId);
            other->_fromOthers.push_back(destId); // Populate cache
            // On success, register that pair
            result = unite(result, Hyperedges{destId, otherId});
        }
    }
    return result;
}

Hyperedges Hypergraph::from(const Hyperedges& ids, const std::string& label) const
{
    Hyperedges result;
    if (label.empty())
    {
        // Fast path for empty labels
        for (const UniqueId& id : ids)
        {
            Hyperedges fromIds(read(id).pointingFrom());
            result = unite(result, fromIds);
        }
    } else {
        for (const UniqueId& id : ids)
        {
            Hyperedges fromIds(read(id).pointingFrom());
            for (const UniqueId& fromId : fromIds)
            {
                if (label.empty() || (read(fromId).label() == label))
                    result.push_back(fromId);
            }
        }
    }
    return result;
}

Hyperedges Hypergraph::to(const Hyperedges& srcIds, const Hyperedges& otherIds)
{
    Hyperedges result;
    for (const UniqueId& srcId : srcIds)
    {
        Hyperedge* srcEdge = get(srcId);
        if (!srcEdge)
            continue;
        for (const UniqueId& otherId : otherIds)
        {
            // Check if other is part of this graph as well
            Hyperedge* other = get(otherId);
            if (!other)
                continue;
            srcEdge->to(otherId);
            other->_toOthers.push_back(srcId); // Populate cache
            // On success, register that pair
            result = unite(result, Hyperedges{srcId, otherId});
        }
    }
    return result;
}

Hyperedges Hypergraph::to(const Hyperedges& ids, const std::string& label) const
{
    Hyperedges result;
    if (label.empty())
    {
        // Fast path for empty labels
        for (const UniqueId& id : ids)
        {
            Hyperedges toIds(read(id).pointingTo());
            result = unite(result, toIds);
        }
    } else {
        for (const UniqueId& id : ids)
        {
            Hyperedges toIds(read(id).pointingTo());
            for (const UniqueId& toId : toIds)
            {
                if (label.empty() || (read(toId).label() == label))
                    result.push_back(toId);
            }
        }
    }
    return result;
}

Hyperedges Hypergraph::prevNeighboursOf(const Hyperedges& ids, const std::string& label) const
{
    Hyperedges result;
    //Hyperedges all = find(label);
    for (const UniqueId& id : ids)
    {
        result = unite(result, from(Hyperedges{id},label));
        for (const UniqueId& other : read(id)._toOthers)
        {
            //Hyperedge *o(get(other));
            // Check cache validity
            //if (!o)
                //continue;
            // Check label
            if (!label.empty() && (label != read(other).label()))
                continue;
            // Check if id is in the TO set of other
            if (!read(other).isPointingTo(id))
                continue;
            result.push_back(other);
        }
    }
    return result;
}

Hyperedges Hypergraph::nextNeighboursOf(const Hyperedges& ids, const std::string& label) const
{
    Hyperedges result;
    //Hyperedges all = find(label);
    for (const UniqueId& id : ids)
    {
        result = unite(result, to(Hyperedges{id},label));
        for (const UniqueId& other : read(id)._fromOthers)
        {
            //Hyperedge *o(get(other));
            // Check cache validity
            //if (!o)
                //continue;
            // Check label
            if (!label.empty() && (label != read(other).label()))
                continue;
            // Check if id is in the TO set of other
            if (!read(other).isPointingFrom(id))
                continue;
            result.push_back(other);
        }
    }
    return result;
}

Hyperedges Hypergraph::allNeighboursOf(const Hyperedges& ids, const std::string& label) const
{
    Hyperedges result;
    result = unite(prevNeighboursOf(ids,label), nextNeighboursOf(ids,label));
    return result;
}

std::ostream& operator<< (std::ostream& os , const Mapping& val)
{
    os << "{ ";
    for (const auto &pair : val)
    {
        os << "(" << pair.first << " , " << pair.second << ") ";
    }
    os << "} ";
    return os;
}

Mapping fromHyperedges(const Hyperedges& a)
{
    Mapping result;
    for (const UniqueId& id : a)
    {
        result.insert({id, id});
    }
    return result;
}

bool equal(const Mapping& a, const Mapping& b)
{
    // For two mappings to be equal, they
    // a) have to have the same size
    if (a.size() != b.size())
        return false;
    // b) for each pair in a:
    for (const auto& pair : a)
    {
        bool found(false);
        // there has to be the same pair in b
        for (const auto& otherPair : b)
        {
            if ((pair.first == otherPair.first) && (pair.second == otherPair.second))
            {
                found = true;
                break;
            }
        }
        if (!found)
            return false;
    }
    return true;
}

Mapping invert(const Mapping& m)
{
    Mapping result;
    for (const auto &pair : m)
    {
        result.insert({pair.second, pair.first});
    }
    return result;
}

Mapping join(const Mapping& a, const Mapping& b)
{
    Mapping result;
    for (const auto& pair : a)
    {
        // In case of a multimap, we have to find all occurences of a.first in b
        for (const auto& otherPair : b)
        {
            if (pair.first != otherPair.first)
                continue;
            result.insert({pair.second, otherPair.second});
        }
    }
    return result;
}
