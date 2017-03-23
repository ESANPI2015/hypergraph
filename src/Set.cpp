#include "Set.hpp"

Set::Set(const std::string& label)
: Hyperedge(label)
{
}

Set::Set(Set::Sets members, const std::string& label)
: Hyperedge(label)
{
    for (auto setIt : members)
    {
        auto set = setIt.second;
        contains(set);
    }
}

Set* Set::promote(Hyperedge *edge)
{
    return static_cast<Set*>(edge);
}

Set::Sets Set::promote(Hyperedge::Hyperedges edges)
{
    Set::Sets result;
    for (auto edgeIt : edges)
    {
        auto edge = edgeIt.second;
        result[edge->id()] = static_cast<Set*>(edge);
    }
    return result;
}

bool Set::contains(Set *other)
{
    // This will create a memberOf Relation
    Relation* memberOf = Relation::create("memberOf");
    bool a = memberOf->from(other);
    bool b = memberOf->to(this);
    return (a && b);
}

bool Set::isA(Set *other)
{
    // This will create an is-a Relation
    Relation* isA = Relation::create("isA");
    bool a = isA->from(this);
    bool b = isA->to(other);
    return (a && b);
}

bool Set::partOf(Set *other)
{
    // This will create a partOf Relation
    Relation* partOf = Relation::create("partOf");
    bool a = partOf->from(this);
    bool b = partOf->to(other);
    return (a && b);
}

Set* Set::create(const std::string& label)
{
    Set* neu = new Set(label);
    _created[neu->_id] = neu; // down-cast
    return neu;
}
