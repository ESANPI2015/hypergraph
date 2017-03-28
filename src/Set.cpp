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
        set->memberOf(this);
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

bool Set::memberOf(Set *other)
{
    // This will create a memberOf Relation
    Relation* memberOf = Relation::create("memberOf");
    bool a = memberOf->from(this);
    bool b = memberOf->to(other);
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

Set* Set::memberOf()
{
    Set *query;
    // This query gives all members of this
    query = traversal<Set>(
        [&](Hyperedge *x){return ((x->id() != this->id()) && (x->label() != "memberOf")) ? true : false;},
        [](Hyperedge *x, Hyperedge *y){return ((x->label() == "memberOf") || (y->label() == "memberOf")) ? true : false;},
        "memberOf",
        DOWN
    );
    // So i will point to this query (which is a new SUPER relation)
    // TODO: We can do this but should get rid of all other 'memberOf' relations we had before (otherwise everything explodes?)
    //pointTo(query);
    return query;
}

Set* Set::members()
{
    Set *query;
    // This query gives all members of this
    query = traversal<Set>(
        [&](Hyperedge *x){return ((x->id() != this->id()) && (x->label() != "memberOf")) ? true : false;},
        [](Hyperedge *x, Hyperedge *y){return ((x->label() == "memberOf") || (y->label() == "memberOf")) ? true : false;},
        "members",
        UP
    );
    return query;
}

Set* Set::kindOf()
{
    Set *query;
    // This query gives all supertypes of this
    query = traversal<Set>(
        [&](Hyperedge *x){return ((x->id() != this->id()) && (x->label() != "isA")) ? true : false;},
        [](Hyperedge *x, Hyperedge *y){return ((x->label() == "isA") || (y->label() == "isA")) ? true : false;},
        "isA",
        DOWN
    );
    // So i will point to this query (which is a new SUPER relation)
    // TODO: We can do this but should get rid of all other 'isA' relations we had before (otherwise everything explodes?)
    //pointTo(query);
    return query;
}

Set* Set::subclasses()
{
    Set *query;
    // This query gives all supertypes of this
    query = traversal<Set>(
        [&](Hyperedge *x){return ((x->id() != this->id()) && (x->label() != "isA")) ? true : false;},
        [](Hyperedge *x, Hyperedge *y){return ((x->label() == "isA") || (y->label() == "isA")) ? true : false;},
        "subclasses",
        UP
    );
    return query;
}

Set* Set::partOf()
{
    Set *query;
    // This query gives all wholes we are part-of
    query = traversal<Set>(
        [&](Hyperedge *x){return ((x->id() != this->id()) && (x->label() != "partOf")) ? true : false;},
        [](Hyperedge *x, Hyperedge *y){return ((x->label() == "partOf") || (y->label() == "partOf")) ? true : false;},
        "partOf",
        DOWN
    );
    // So i will point to this query (which is a new SUPER relation)
    // TODO: We can do this but should get rid of all other 'partOf' relations we had before (otherwise everything explodes?)
    //pointTo(query);
    return query;
}

Set* Set::parts()
{
    Set *query;
    // This query gives all parts of a whole
    query = traversal<Set>(
        [&](Hyperedge *x){return ((x->id() != this->id()) && (x->label() != "partOf")) ? true : false;},
        [](Hyperedge *x, Hyperedge *y){return ((x->label() == "partOf") || (y->label() == "partOf")) ? true : false;},
        "parts",
        UP
    );
    return query;
}
