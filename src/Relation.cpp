#include "Relation.hpp"
#include "Set.hpp"

Relation::Relation(const std::string& label)
: Hyperedge(label)
{
}

Relation::Relation(Hyperedge::Hyperedges from, Hyperedge::Hyperedges to, const std::string& label)
: Hyperedge(to, label)
{
    for (auto fromId : from)
    {
        auto other = Hyperedge::find(fromId);
        other->pointTo(_id);
    }
}

bool Relation::from(const unsigned id)
{
    return Hyperedge::find(id)->pointTo(_id);
}

bool Relation::to(const unsigned id)
{
    return pointTo(id);
}

bool Relation::from(Set *other)
{
    return other->pointTo(_id);
}

bool Relation::to(Set *other)
{
    return pointTo(other->id());
}

Relation* Relation::create(const std::string& label)
{
    Relation* neu = static_cast<Relation*>(Hyperedge::create(label));
    return neu;
}
