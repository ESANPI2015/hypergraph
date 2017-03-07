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
    _members.insert(_members.end(), members.begin(), members.end());
}

std::string Hyperedge::label() const
{
    return _label;
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

Hyperedge Hyperedge::query(const std::string& label, const unsigned size, const std::string& name)
{
    Hyperedges members;
    std::set< Hyperedge* > visited;
    std::queue< Hyperedge* > edges;

    edges.push(this);

    // Run through queue of unknown edges
    while (!edges.empty())
    {
        auto edge = edges.front();
        auto edgeMembers = edge->members();
        edges.pop();

        if (visited.count(edge))
            continue;

        // Visiting!!!
        visited.insert(edge);
        // TODO: In a generic algorithm you would call here a lambda or such
        if ((label.empty()
            || (edge->label().find(label) != std::string::npos))
            && (edgeMembers.size() <= size)
           )
        {
            members.push_back(edge);
        }

        // Inserting members into queue
        for (auto unknown : edgeMembers)
        {
            edges.push(unknown);
        }
    }

    return Hyperedge(members, name);
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
            (*known[label]) = Hyperedge(members, label);
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
