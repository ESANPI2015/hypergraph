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

bool Set::contains(Set *other)
{
    // This will create a memberOf Relation
    Relation* memberOf = Relation::create("memberOf");
    bool a = memberOf->from(other);
    bool b = memberOf->to(this);
    return (a && b);
}
