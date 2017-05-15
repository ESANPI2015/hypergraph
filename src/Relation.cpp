#include "Relation.hpp"
#include "Set.hpp"

Relation* Relation::promote(Hyperedge *edge)
{
    Relation* neu = static_cast<Relation*>(edge);
    // TODO: isA* relation
    return neu;
}

bool Relation::from(const unsigned id)
{
    return Hyperedge::find(id)->pointTo(this->id());
}

bool Relation::to(const unsigned id)
{
    return pointTo(id);
}

bool Relation::from(Set *other)
{
    return other->pointTo(this->id());
}

bool Relation::to(Set *other)
{
    return pointTo(other->id());
}

Relation* Relation::create(const std::string& label)
{
    Relation* neu = Relation::promote(Hyperedge::create(label));
    return neu;
}

Relation* create(Hyperedge::Hyperedges from, Hyperedge::Hyperedges to, const std::string& label)
{
    Relation* neu = Relation::promote(Hyperedge::create(to, label));
    for (auto fromId : from)
    {
        neu->from(fromId);
    }
    return neu;
}
