#include "Set.hpp"
#include <stdexcept>

Set::Set(Hyperedge::Hyperedges members, const std::string& label)
: Hyperedge(members, label)
{
    if (members.size() < 1)
    {
        throw std::invalid_argument("A valid set needs at least one member");
    }
}
