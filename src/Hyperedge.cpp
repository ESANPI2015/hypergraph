#include "Hyperedge.hpp"
#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <queue>

Hyperedge::Hyperedge(const std::string& label)
: _label(label)
{
}

Hyperedge::Hyperedge(Hyperedges members, const std::string& label)
{
    _label = label;
    for (auto member : members)
    {
        contains(member);
    }
}

bool Hyperedge::contains(Hyperedge *member)
{
    auto nomember = member;
    for (auto mine : _members)
    {
        if (mine == nomember)
            return true;
    }
        // Check if we are in the super sets of member
        bool found = false;
        for (auto super : nomember->_supers)
        {
            if (super == this)
            {
                found = true;
                break;
            }
        }

        // Make sure we are in the super list
        if (!found)
        {
            nomember->_supers.push_back(this);
        }
        // ... and the member registered in our member list
        _members.push_back(nomember);
    
    return true;
}

std::string Hyperedge::label() const
{
    return _label;
}

Hyperedge::Hyperedges Hyperedge::supers(const std::string& label)
{
    Hyperedges result;
    for (auto edge : _supers)
    {
        // Filters by label if given. It suffices that the edge label contains the given one.
        if (label.empty() || (edge->label().find(label) != std::string::npos))
            result.push_back(edge);
    }
    return result;
}

Hyperedge::Hyperedges Hyperedge::members(const std::string& label)
{
    Hyperedges result;
    for (auto edge : _members)
    {
        // Filters by label if given. It suffices that the edge label contains the given one.
        if (label.empty() || (edge->label().find(label) != std::string::npos))
            result.push_back(edge);
    }
    return result;
}


Hyperedge::Hyperedges Hyperedge::labelContains(const std::string& str, const Hyperedge::TraversalDirection dir)
{
    return traversal(
        [&](Hyperedge *x){ return (str.empty() || (x->label().find(str) != std::string::npos)) ? true : false; },
        dir
    );
}

Hyperedge::Hyperedges Hyperedge::labelPartOf(const std::string& str, const Hyperedge::TraversalDirection dir)
{
    return traversal(
        [&](Hyperedge *x){ return (str.empty() || (str.find(x->label()) != std::string::npos)) ? true : false; },
        dir
    );
}

Hyperedge::Hyperedges Hyperedge::cardinalityLessThanOrEqual(const unsigned cardinality, const Hyperedge::TraversalDirection dir)
{
    return traversal(
        [&](Hyperedge *x){ return (x->members().size() <= cardinality)? true : false; },
        dir
    );
}

Hyperedge::Hyperedges Hyperedge::cardinalityGreaterThan(const unsigned cardinality, const Hyperedge::TraversalDirection dir)
{
    return traversal(
        [&](Hyperedge *x){ return (x->members().size() > cardinality)? true : false; },
        dir
    );
}

template <typename Func> Hyperedge::Hyperedges Hyperedge::traversal(Func f, const Hyperedge::TraversalDirection dir)
{
    Hyperedges members;
    std::set< Hyperedge* > visited;
    std::queue< Hyperedge* > edges;

    edges.push(this);

    // Run through queue of unknown edges
    while (!edges.empty())
    {
        auto edge = edges.front();
        edges.pop();

        // Handle search direction
        Hyperedges unknowns;
        switch (dir)
        {
            case DOWN:
                unknowns.insert(unknowns.end(), edge->_members.begin(), edge->_members.end());
                break;
            case BOTH:
                unknowns.insert(unknowns.end(), edge->_members.begin(), edge->_members.end());
            case UP:
                unknowns.insert(unknowns.end(), edge->_supers.begin(), edge->_supers.end());
                break;
            default:
                members.clear();
                return members;
        }

        if (visited.count(edge))
            continue;

        // Visiting!!!
        visited.insert(edge);
        if (f(edge))
        {
            members.push_back(edge);
        }

        // Inserting members and supers into queue
        for (auto unknown : unknowns)
        {
            edges.push(unknown);
        }
    }

    return members;
}

std::string Hyperedge::serialize(Hyperedge* root)
{
    std::stringstream result;
    std::set< Hyperedge* > visited;
    std::queue< Hyperedge* > edges;

    edges.push(root);

    // Run through queue of unknown edges
    while (!edges.empty())
    {
        auto edge = edges.front();
        edges.pop();

        if (visited.count(edge))
            continue;

        // Visiting!!!
        visited.insert(edge);
        result << edge->label();

        // Inserting members into queue
        for (auto unknown : edge->members())
        {
            result << "[" << unknown->label() << "]";
            edges.push(unknown);
        }
        result << "\n";
    }

    return result.str();
}

Hyperedge* Hyperedge::deserialize(const std::string& from)
{
    Hyperedge *root = NULL;
    std::map< std::string, Hyperedge* > known;
    std::set< std::string > roots;
    
    auto spos = 0;
    auto opos = from.find("[");
    auto npos = from.find("\n");

    // Run through string
    while (npos < from.size())
    {
        // Extract label
        auto label = from.substr(spos, npos-spos);
        if (opos < npos)
            label = from.substr(spos, opos-spos);

        // Go through all members, create them if necessary and update current hyperedge
        Hyperedge::Hyperedges members;
        while (opos < npos)
        {
            auto member = from.substr(opos+1, from.find("]",opos+1)-opos-1);
            // Find label in map (and create it iff not found)
            if (!known.count(member))
            {
                known[member] = new Hyperedge(member);
            }
            members.push_back(known[member]);
            // Whenever something gets a member, it cannot be a root anymore
            if (roots.count(member))
            {
                roots.erase(member);
            }
            opos = from.find("[", opos+1);
        }

        // Find label in map, create or update it
        if (!known.count(label))
        {
            known[label] = new Hyperedge(members, label);
            // Whenever a parent is created it might be a root
            roots.insert(label);
        } else {
            // Update already existing hyperedge
            for (auto member : members)
            {
                known[label]->contains(member);
            }
        }

        // Update positions
        spos = npos+1;
        opos = from.find("[", spos);
        npos = from.find("\n",spos);
    }

    // Now we have to find the root
    if (roots.size() > 1)
        std::cout << "Multiple roots?!" << std::endl;
    if (roots.size() < 1)
        std::cout << "No roots?!" << std::endl;

    root = known[*(roots.begin())];

    return root;
}
