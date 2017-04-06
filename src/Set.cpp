#include "Set.hpp"

Set::Set(const std::string& label)
: Hyperedge(label)
{
}

Set::Set(Set::Sets members, const std::string& label)
: Hyperedge(label)
{
    for (auto setId : members)
    {
        auto set = Set::promote(Hyperedge::find(setId));
        set->memberOf(this);
    }
}

Set* Set::promote(Hyperedge *edge)
{
    return static_cast<Set*>(edge);
}

Set::Sets Set::promote(Hyperedge::Hyperedges edges)
{
    //Set::Sets result;
    //for (auto edgeIt : edges)
    //{
    //    auto edge = edgeIt.second;
    //    result[edge->id()] = static_cast<Set*>(edge);
    //}
    //return result;
    return edges;
}

bool Set::memberOf(Set *other)
{
    bool result = true;
    auto edges = pointingTo("memberOf");
    Relation *memberOf = NULL;
    if (edges.size())
    {
        // There already exists at least one memberOf relation
        memberOf = static_cast< Relation* >(Hyperedge::find(*edges.begin()));
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
        isA = static_cast< Relation* >(Hyperedge::find(*edges.begin()));
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
        partOf = static_cast< Relation* >(Hyperedge::find(*edges.begin()));
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
    Set* neu = Set::promote(Hyperedge::create(label));
    return neu;
}

Set* Set::create(Set::Sets members, const std::string& label)
{
    Set* neu = Set::promote(Hyperedge::create(members, label));
    return neu;
}

Relation* Set::memberOf()
{
    Relation *query;
    // This query gives all members of this
    query = traversal<Relation>(
        [&](Hyperedge *x){return ((x->id() != this->id()) && (x->label() != "memberOf")) ? true : false;},
        [](Hyperedge *x, Hyperedge *y){return ((x->label() == "memberOf") || (y->label() == "memberOf")) ? true : false;},
        "memberOf",
        DOWN
    );
    // So i will point to this query (which is a new SUPER relation)
    pointTo(query->id());
    return query;
}

Relation* Set::kindOf()
{
    Relation *query;
    // This query gives all supertypes of this
    query = traversal<Relation>(
        [&](Hyperedge *x){return ((x->id() != this->id()) && (x->label() != "isA")) ? true : false;},
        [](Hyperedge *x, Hyperedge *y){return ((x->label() == "isA") || (y->label() == "isA")) ? true : false;},
        "isA",
        DOWN
    );
    // So i will point to this query (which is a new SUPER relation)
    pointTo(query->id());
    return query;
}

Relation* Set::partOf()
{
    Relation *query;
    // This query gives all wholes we are part-of
    query = traversal<Relation>(
        [&](Hyperedge *x){return ((x->id() != this->id()) && (x->label() != "partOf")) ? true : false;},
        [](Hyperedge *x, Hyperedge *y){return ((x->label() == "partOf") || (y->label() == "partOf")) ? true : false;},
        "partOf",
        DOWN
    );
    // So i will point to this query (which is a new SUPER relation)
    pointTo(query->id());
    return query;
}

Relation* Set::setOf()
{
    Relation *query;
    // This query gives all members of this (transitive as well)
    query = traversal<Relation>(
        [&](Hyperedge *x){return ((x->id() != this->id()) && (x->label() != "memberOf")) ? true : false;},
        [](Hyperedge *x, Hyperedge *y){return ((x->label() == "memberOf") || (y->label() == "memberOf")) ? true : false;},
        "setOf",
        UP
    );
    // So i will point to this query (which is a new SUPER relation)
    pointTo(query->id());
    return query;
}

Relation* Set::superclassOf()
{
    Relation *query;
    // This query gives all supertypes of this
    query = traversal<Relation>(
        [&](Hyperedge *x){return ((x->id() != this->id()) && (x->label() != "isA")) ? true : false;},
        [](Hyperedge *x, Hyperedge *y){return ((x->label() == "isA") || (y->label() == "isA")) ? true : false;},
        "superclassOf",
        UP
    );
    // So i will point to this query (which is a new SUPER relation)
    pointTo(query->id());
    return query;
}

Relation* Set::wholeOf()
{
    Relation *query;
    // This query gives all parts of a whole
    query = traversal<Relation>(
        [&](Hyperedge *x){return ((x->id() != this->id()) && (x->label() != "partOf")) ? true : false;},
        [](Hyperedge *x, Hyperedge *y){return ((x->label() == "partOf") || (y->label() == "partOf")) ? true : false;},
        "wholeOf",
        UP
    );
    // So i will point to this query (which is a new SUPER relation)
    pointTo(query->id());
    return query;
}

Set::Sets Set::members(const std::string& label) const
{
    Set::Sets result;
    Hyperedge::Hyperedges rels = this->pointedBy("memberOf"); // Gives all memberOf relations pointing to us

    for (auto relId : rels)
    {
        Relation *rel = static_cast<Relation*>(Hyperedge::find(relId));
        // Get all sets containing this relation AND having a certain label
        Set::Sets others = Set::promote(rel->pointedBy(label));
        // Merge them with the current result map
        result.insert(others.begin(), others.end());
    }

    return result;
}

Set* Set::unite(const Set* other)
{
    auto mine = this->members();
    auto others = other->members();
    mine.insert(others.begin(), others.end());
    return Set::create(mine, this->label() + " U " + other->label());
}

Set* Set::intersect(const Set* other)
{
    Set::Sets result;
    auto mine = this->members();
    auto others = other->members();
    for (auto mineId : mine)
    {
        if (others.count(mineId))
        {
            result.insert(mineId);
        }
    }
    return Set::create(result, this->label() + " ^ " + other->label());
}

Set* Set::subtract(const Set* other)
{
    Set::Sets result;
    auto mine = this->members();
    auto others = other->members();
    for (auto mineId : mine)
    {
        if (!others.count(mineId))
        {
            result.insert(mineId);
        }
    }
    return Set::create(result, this->label() + " \\ " + other->label());
}
