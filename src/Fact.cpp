#include "Fact.hpp"
#include <stdexcept>

Fact::Fact(Hyperedge::Hyperedges members, const std::string& label)
: Set(members, label)
{
    if (members.size() < 2)
    {
        throw std::invalid_argument("A valid fact needs at least two members");
    }
    for (auto member : members)
    {
        if (member->members().size())
            throw std::invalid_argument("Member " + member->label() + " of fact is not a 0-hyperedge");
    }
}
