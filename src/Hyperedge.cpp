#include "Hyperedge.hpp"
#include <sstream>

Hyperedge::Hyperedge(const std::string& label)
: _label(label)
{}

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

std::string Hyperedge::serialize(unsigned lvl, const std::string& delimiter)
{
    /*DFS*/
    std::stringstream result;
    result << std::string(lvl, '\t') << label() << delimiter;
    for (auto edge : members())
    {
        result << edge->serialize(lvl+1,delimiter);
    }
    return result.str();
}
