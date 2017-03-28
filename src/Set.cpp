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
    bool result = true;
    auto edges = pointingTo("memberOf");
    Relation *memberOf = NULL;
    if (edges.size())
    {
        // There already exists a memberOf relation
        memberOf = static_cast< Relation* >(edges.begin()->second);
        result &= memberOf->to(other);
    } else {
        // This will create a memberOf Relation
        memberOf = Relation::create("memberOf");
        result &= memberOf->from(this);
        result &= memberOf->to(other);
    }
    return result;
}

bool Set::isA(Set *other)
{
    bool result = true;
    auto edges = pointingTo("isA");
    Relation *isA = NULL;
    if (edges.size())
    {
        // There already exists a isA relation
        isA = static_cast< Relation* >(edges.begin()->second);
        result &= isA->to(other);
    } else {
        // This will create a isA Relation
        isA = Relation::create("isA");
        result &= isA->from(this);
        result &= isA->to(other);
    }
    return result;
}

bool Set::partOf(Set *other)
{
    bool result = true;
    auto edges = pointingTo("partOf");
    Relation *partOf = NULL;
    if (edges.size())
    {
        // There already exists a partOf relation
        partOf = static_cast< Relation* >(edges.begin()->second);
        result &= partOf->to(other);
    } else {
        // This will create a partOf Relation
        partOf = Relation::create("partOf");
        result &= partOf->from(this);
        result &= partOf->to(other);
    }
    return result;
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
