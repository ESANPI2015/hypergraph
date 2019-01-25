#include "Hypergraph.hpp"

#include <iostream>

const UniqueId Hypergraph::Zero = "Hypergraph::Hyperedge::Zero";

Hypergraph::Hypergraph()
{
    create(Zero, "ZERO");
}

Hypergraph::Hypergraph(const Hypergraph& other)
{
    create(Zero, "ZERO");
    importFrom(other);
}

Hypergraph::~Hypergraph()
{
    // We hold no pointers so we do not need to do anything here
}

void Hypergraph::importFrom(const Hypergraph& other)
{
    // When we construct a graph from other, we have to repopulate the cache by rewiring
    // First pass: Clone hedges
    for (const UniqueId& id : other.findByLabel())
    {
        create(id, other._edges.at(id).label());
    }

    // Second pass: Rewire (see from and to methods which populate the cache(s))
    for (const UniqueId id : findByLabel())
    {
        if (!other._edges.count(id))
            continue;
        // Wire only those things which have not yet been wired before (otherwise we get arity changes)
        Hyperedges newFromUids(subtract(other._edges.at(id).pointingFrom(), _edges.at(id).pointingFrom()));
        Hyperedges newToUids(subtract(other._edges.at(id).pointingTo(), _edges.at(id).pointingTo()));
        pointsFrom(Hyperedges{id}, newFromUids);
        pointsTo(Hyperedges{id}, newToUids);
    }
}

Hyperedges Hypergraph::create(const UniqueId id, const std::string& label)
{
    if (!exists(id))
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
    if (!exists(id))
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
    // We point to others and others might point to us
    // I. In all Hyperedges WE point to or from we have to cleanup the caches
    Hyperedges fromIds(access(id)._from);
    for (const UniqueId& fromId : fromIds)
    {
        Hyperedge& other(access(fromId));
        if (other.id() == Zero)
            continue;
        other._fromOthers.erase(std::remove(other._fromOthers.begin(), other._fromOthers.end(), id), other._fromOthers.end());
    }
    Hyperedges toIds(access(id)._to);
    for (const UniqueId& toId : toIds)
    {
        Hyperedge& other(access(toId));
        if (other.id() == Zero)
            continue;
        other._toOthers.erase(std::remove(other._toOthers.begin(), other._toOthers.end(), id), other._toOthers.end());
    }
    // II. In all Hyperedges which point to or from US we have to cleanup their from and to sets
    Hyperedges fromUsIds(access(id)._fromOthers);
    for (const UniqueId& fromUsId : fromUsIds)
    {
        Hyperedge& other(access(fromUsId));
        if (other.id() == Zero)
            continue;
        other._from.erase(std::remove(other._from.begin(), other._from.end(), id), other._from.end());
    }
    Hyperedges toUsIds(access(id)._toOthers);
    for (const UniqueId& toUsId : toUsIds)
    {
        Hyperedge& other(access(toUsId));
        if (other.id() == Zero)
            continue;
        other._to.erase(std::remove(other._to.begin(), other._to.end(), id), other._to.end());
    }
}

bool Hypergraph::exists(const UniqueId& uid) const
{
    if (_edges.count(uid))
        return true;
    return false;
}

const Hyperedge& Hypergraph::access(const UniqueId id) const
{
    return _edges.at(id);
}

Hyperedge& Hypergraph::access(const UniqueId id)
{
    if (exists(id))
    {
        return _edges.at(id);
    } else {
        return _edges.at(Hypergraph::Zero);
    }
}

Hyperedges Hypergraph::findByLabel(const std::string& label) const
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


Hyperedges Hypergraph::pointsFrom(const Hyperedges& destIds, const Hyperedges& otherIds)
{
    Hyperedges result;
    for (const UniqueId& destId : destIds)
    {
        Hyperedge& destEdge(access(destId));
        if (destEdge.id() == Zero)
            continue;
        for (const UniqueId& otherId : otherIds)
        {
            // Check if other is part of this graph as well
            Hyperedge& other(access(otherId));
            if (other.id() == Zero)
                continue;
            destEdge.pointsFrom(otherId);
            other._fromOthers.push_back(destId); // Populate cache
            // On success, register that pair
            result = unite(result, Hyperedges{destId, otherId});
        }
    }
    return result;
}

Hyperedges Hypergraph::isPointingFrom(const Hyperedges& ids, const std::string& label) const
{
    Hyperedges result;
    if (label.empty())
    {
        // Fast path for empty labels
        for (const UniqueId& id : ids)
        {
            Hyperedges fromIds(access(id).pointingFrom());
            result = unite(result, fromIds);
        }
    } else {
        for (const UniqueId& id : ids)
        {
            Hyperedges fromIds(access(id).pointingFrom());
            for (const UniqueId& fromId : fromIds)
            {
                if (access(fromId).label() == label)
                    result.push_back(fromId);
            }
        }
    }
    return result;
}

Hyperedges Hypergraph::pointsTo(const Hyperedges& srcIds, const Hyperedges& otherIds)
{
    Hyperedges result;
    for (const UniqueId& srcId : srcIds)
    {
        Hyperedge& srcEdge(access(srcId));
        if (srcEdge.id() == Zero)
            continue;
        for (const UniqueId& otherId : otherIds)
        {
            // Check if other is part of this graph as well
            Hyperedge& other(access(otherId));
            if (other.id() == Zero)
                continue;
            srcEdge.pointsTo(otherId);
            other._toOthers.push_back(srcId); // Populate cache
            // On success, register that pair
            result = unite(result, Hyperedges{srcId, otherId});
        }
    }
    return result;
}

Hyperedges Hypergraph::isPointingTo(const Hyperedges& ids, const std::string& label) const
{
    Hyperedges result;
    if (label.empty())
    {
        // Fast path for empty labels
        for (const UniqueId& id : ids)
        {
            Hyperedges toIds(access(id).pointingTo());
            result = unite(result, toIds);
        }
    } else {
        for (const UniqueId& id : ids)
        {
            Hyperedges toIds(access(id).pointingTo());
            for (const UniqueId& toId : toIds)
            {
                if (label.empty() || (access(toId).label() == label))
                    result.push_back(toId);
            }
        }
    }
    return result;
}

Hyperedges Hypergraph::previousNeighboursOf(const Hyperedges& ids, const std::string& label) const
{
    Hyperedges result;
    //Hyperedges all = findByLabel(label);
    for (const UniqueId& id : ids)
    {
        result = unite(result, isPointingFrom(Hyperedges{id},label));
        for (const UniqueId& other : access(id)._toOthers)
        {
            // Check label
            if (!label.empty() && (label != access(other).label()))
                continue;
            // Check if id is in the TO set of other
            if (!access(other).isPointingTo(id))
                continue;
            result.push_back(other);
        }
    }
    return result;
}

Hyperedges Hypergraph::nextNeighboursOf(const Hyperedges& ids, const std::string& label) const
{
    Hyperedges result;
    //Hyperedges all = findByLabel(label);
    for (const UniqueId& id : ids)
    {
        result = unite(result, isPointingTo(Hyperedges{id},label));
        for (const UniqueId& other : access(id)._fromOthers)
        {
            // Check label
            if (!label.empty() && (label != access(other).label()))
                continue;
            // Check if id is in the TO set of other
            if (!access(other).isPointingFrom(id))
                continue;
            result.push_back(other);
        }
    }
    return result;
}

Hyperedges Hypergraph::allNeighboursOf(const Hyperedges& ids, const std::string& label) const
{
    Hyperedges result;
    result = unite(previousNeighboursOf(ids,label), nextNeighboursOf(ids,label));
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
