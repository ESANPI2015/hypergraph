#include "Hypergraph.hpp"
#include "Hyperedge.hpp"

#include <iostream>
#include <stack>

Hypergraph::Hypergraph()
{
}

Hypergraph::Hypergraph(Hypergraph& A, Hypergraph& B)
{
    // If things have the same ID they are the same!
    Hyperedges allOfA = A.find();
    Hyperedges allOfB = B.find();

    // First pass: Clone hedges of A and B
    for (auto idA : allOfA)
    {
        create(idA, A.get(idA)->label());
    }
    for (auto idB : allOfB)
    {
        create(idB, B.get(idB)->label());
    }

    // Second pass: Wire the hedges
    Hyperedges allOfMe = find();
    for (auto id : allOfMe)
    {
        // The new hedges will point to/from the union of the corresponding sets of the A and B hedges with the same id
        Hyperedges fromA = A.get(id) ? A.get(id)->pointingFrom() : Hyperedges();
        Hyperedges fromB = B.get(id) ? B.get(id)->pointingFrom() : Hyperedges();
        Hyperedges toA = A.get(id) ? A.get(id)->pointingTo() : Hyperedges();
        Hyperedges toB = B.get(id) ? B.get(id)->pointingTo() : Hyperedges();
        from(unite(fromA, fromB), id);
        to(id, unite(toA, toB));
    }
}

Hypergraph::~Hypergraph()
{
    // We hold no pointers so we do not need to do anything here
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

Hypergraph::Hyperedges Hypergraph::find(const std::string& label)
{
    Hyperedges result;
    for (auto pair : _edges)
    {
        auto id = pair.first;
        auto edge = pair.second;
        // If edge does not match the label, skip it
        if (!label.empty() && (edge.label() != label))
            continue;
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

Hypergraph::Hyperedges Hypergraph::from(const unsigned id, const std::string& label)
{
    Hyperedges result;
    Hyperedges fromIds = get(id)->pointingFrom();
    for (auto fromId : fromIds)
    {
        if (label.empty() || (get(fromId)->label() == label))
            result.insert(fromId);
    }
    return result;
}

Hypergraph::Hyperedges Hypergraph::to(const unsigned id, const std::string& label)
{
    Hyperedges result;
    Hyperedges toIds = get(id)->pointingTo();
    for (auto toId : toIds)
    {
        if (label.empty() || (get(toId)->label() == label))
            result.insert(toId);
    }
    return result;
}

Hypergraph::Hyperedges Hypergraph::from(const Hyperedges& ids, const std::string& label)
{
    Hyperedges result;
    for (auto id : ids)
    {
        auto fromIds = from(id, label);
        result.insert(fromIds.begin(), fromIds.end());
    }
    return result;
}

Hypergraph::Hyperedges Hypergraph::to(const Hyperedges& ids, const std::string& label)
{
    Hyperedges result;
    for (auto id : ids)
    {
        auto toIds = to(id, label);
        result.insert(toIds.begin(), toIds.end());
    }
    return result;
}

Hypergraph::Hyperedges Hypergraph::unite(const Hyperedges& edgesA, const Hyperedges& edgesB)
{
    Hyperedges result(edgesA);
    result.insert(edgesB.begin(), edgesB.end());
    return result;
}

Hypergraph::Hyperedges Hypergraph::intersect(const Hyperedges& edgesA, const Hyperedges& edgesB)
{
    Hyperedges edgesC;
    for (auto id : edgesA)
    {
        if (edgesB.count(id))
        {
            edgesC.insert(id);
        }
    }
    return edgesC;
}

Hypergraph::Hyperedges Hypergraph::subtract(const Hyperedges& edgesA, const Hyperedges& edgesB)
{
    Hyperedges edgesC;
    for (auto id : edgesA)
    {
        if (!edgesB.count(id))
        {
            edgesC.insert(id);
        }
    }
    return edgesC;
}

Hypergraph::Hyperedges Hypergraph::neighboursOf(const unsigned id, const std::string& label)
{
    Hyperedges result;
    result = unite(from(id,label), to(id,label));
    Hyperedges all = find();
    for (unsigned other : all)
    {
        // Label matching
        if (!label.empty() && (get(other)->label() != label))
            continue;
        // Check if id is in the TO or the FROM set of other
        if (get(other)->isPointingTo(id) || get(other)->isPointingFrom(id))
            result.insert(other);
    }
    return result;
}

Hypergraph::Hyperedges Hypergraph::neighboursOf(const Hyperedges& ids, const std::string& label)
{
    Hyperedges result;
    for (unsigned id : ids)
    {
        Hyperedges n = neighboursOf(id, label);
        result.insert(n.begin(), n.end());
    }
    return result;
}

Hypergraph::Hyperedges Hypergraph::match(Hypergraph& other)
{
    /*
        IDEA:
        I. Select one hyperedge x in other
        II. Get candidates C of x in our graph (first label matching)
        III. For each c in C:
            a) Initialize stack with <x,c>.
            b) Perform a DFS in both graphs simultaneously
    */
    unsigned matches = other.find().size();
    unsigned selectedId = *(other.find().begin());
    std::cout << "\tselected: " << selectedId << "\n";
    Hyperedges candidateIds = find(other.get(selectedId)->label());
    std::cout << "\t#candidates: " << candidateIds.size() << std::endl;
    for (unsigned candidateId : candidateIds)
    {
        Hyperedges result;
        Hyperedges visited;
        std::stack< std::pair<unsigned,unsigned> > toVisit; //III(a)
        toVisit.push(std::pair<unsigned, unsigned>(selectedId,candidateId));
        std::cout << "\tpair: " << selectedId << " " << candidateId << std::endl;
        while (!toVisit.empty())
        {
            // Get the top of the stack
            auto top = toVisit.top();
            toVisit.pop();
            Hyperedge* current = other.get(top.first);
            Hyperedge* candidate = get(top.second);

            // Visiting!!! If already visited, get the next pair from stack
            if (visited.count(current->id()))
                continue;
            visited.insert(current->id());

            std::cout << "\tpair: " << current->id() << " " << candidate->id() << std::endl;
            // Matching. If no match skip everything else!
            if (!current->label().empty() && (current->label() != candidate->label()))
            {
                // Mismatch!
                std::cout << "Mismatch\n";
                continue;
            } else {
                // Match!
                std::cout << "Match\n";
                result.insert(candidate->id());
                if (result.size() >= matches)
                    break;
            }

            // Get the NEIGHBOURS of both current and candidate and put them into stack if they match
            Hyperedges neighbourIds = other.neighboursOf(current->id());
            Hyperedges candidateNeighbourIds = neighboursOf(candidate->id());
            if (neighbourIds.size() > candidateNeighbourIds.size())
            {
                std::cout << "Match impossible\n";
                break;
            }
            std::cout << "\t#neighbours: " << neighbourIds.size() << " " << candidateNeighbourIds.size() << std::endl;
            for (Hyperedges::iterator fit = neighbourIds.begin(); fit != neighbourIds.end(); ++fit)
            {
                unsigned neighbourId = *fit;
                Hyperedge* neighbour = other.get(neighbourId);
                if (visited.count(neighbourId))
                    continue;
                for (Hyperedges::iterator sit = candidateNeighbourIds.begin(); sit != candidateNeighbourIds.end(); ++sit)
                {
                    unsigned candidateNeighbourId = *sit;
                    Hyperedge* candidatesNeighbour = get(candidateNeighbourId);
                    std::cout << "\tpair: " << neighbourId << " " << candidateNeighbourId << std::endl;
                    //toVisit.push(std::pair<unsigned,unsigned>(neighbourId, candidateNeighbourId));
                    // Matching. If they match, add them to the stack
                    if (neighbour->label().empty() || (neighbour->label() == candidatesNeighbour->label()))
                    {
                        std::cout << "Forward Match\n";
                        toVisit.push(std::pair<unsigned,unsigned>(neighbourId, candidateNeighbourId));
                        // ... additionally we have to erase them from sets such that we ensure one-to-one mapping
                        //candidateNeighbourIds.erase(sit);
                        //neighbourIds.erase(fit);
                        //break;
                    }
                }
            }
        }
        // If the result set is of the size of the other graph, we found a match
        if (result.size() == matches)
        {
            // Success :)
            std::cout << "Match found\n";
            return result;
        } else {
            std::cout << "Match not found\n";
        }
    }
    return Hyperedges();
}
