#include "Relation.hpp"

Relation::Relation(const std::string& label)
: Hyperedge(label)
{
}

Relation::Relation(Hyperedge::Hyperedges from, Hyperedge::Hyperedges to, const std::string& label)
: Hyperedge(to, label)
{
    for (auto fromIt : from)
    {
        auto other = fromIt.second;
        other->pointTo(this);
    }
}

bool Relation::from(Hyperedge *source)
{
    return source->pointTo(this);
}

bool Relation::to(Hyperedge *target)
{
    return pointTo(target);
}

Relation* Relation::create(const std::string& label)
{
    Relation* neu = new Relation(label);
    _created[neu->_id] = neu; // down-cast
    return neu;
}
