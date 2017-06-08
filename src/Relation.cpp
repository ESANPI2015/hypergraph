#include "Relation.hpp"

Relation::Relation(const unsigned id, const std::string& label)
: Hyperedge(id,label)
{
}

bool Relation::isValid()
{
    return (indegree() + outdegree() >= 1) ? true : false;
}
