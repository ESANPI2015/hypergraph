#include "Hyperedge.hpp"

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
        // TODO: Maybe just checking if label is contained in other label is more useful
        if (label.empty() || (label == edge->label()))
            result.push_back(edge);
    }
    return result;
}
