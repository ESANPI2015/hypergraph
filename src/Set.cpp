#include "Set.hpp"

Set::Set(const std::string& label)
: Hyperedge(label)
{
}

Set::Set(Hyperedge::Hyperedges members, const std::string& label)
: Hyperedge(members, label)
{
    // Here we could check for set properties
}
