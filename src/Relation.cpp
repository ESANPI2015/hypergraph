#include "Relation.hpp"

Relation::Relation(const std::string& label)
: Set(label)
{
}

Relation::Relation(Hyperedge::Hyperedges members, const std::string& label)
: Set(members, label)
{
    //for (auto member : members)
    //{
        // Here we could check for Relation Properties :)
    //}
}
