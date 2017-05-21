#include "SetSystem.hpp"
#include "Set.hpp"
#include "Relation.hpp"
#include <iostream>

SetSystem::SetSystem()
{
    // Nothing to do?
}

SetSystem::~SetSystem()
{
    // Do we have to do something here?
}

unsigned SetSystem::getSetClass()
{
    auto edges = Hypergraph::find(Set::superclassLabel);
    unsigned id;
    if (edges.size())
    {
        // There are possible representatives: choose one, merge them into one?
        id = *edges.begin();
    } else {
        // There are no possible representatives: create one
        id = Hypergraph::create(Set::superclassLabel);
    }
    return id;
}

unsigned SetSystem::getClass(const std::string& label)
{
    // NOTE: This works on sets, so all derived classes based on the set concept can use this function (Sets could, but this would imply a isA loop!)
    auto sets = find(label);
    unsigned id;
    if (sets.size())
    {
        // There are possible representatives: choose one, merge them into one?
        id = *sets.begin();
    } else {
        // There are no possible representatives: create one
        id = create(label);
    }
    return id;
}

/*Factory functions*/
unsigned SetSystem::create(const std::string& label)
{
    Set *neu = new Set(getNextId(), label);
    _edges[neu->id()] = neu;
    relateTo(neu->id(), getSetClass(), Relation::isALabel);
    return neu->id();
}

unsigned SetSystem::create(Sets members, const std::string& label)
{
    auto id = create(label);
    for (auto memberId : members)
    {
        relateTo(memberId, id, Relation::memberOfLabel);
    }
    return id;
}

bool SetSystem::create(const unsigned id, const std::string& label)
{
    Hyperedge* neu = Hypergraph::get(id);
    if (!neu)
    {
        // Create a new hyperedge
        // Give it the desired id
        Set* set = new Set(id, label);
        _edges[id] = set;
        relateTo(set->id(), getSetClass(), Relation::isALabel);
        return true;
    }
    return false;
}

void SetSystem::destroy(const unsigned id)
{
    Set *set = get(id);
    if (!set) return;

    // Kill everything pointing from/to the set
    for (auto edgeId : set->pointingTo())
    {
        Hypergraph::destroy(edgeId);
    }
    for (auto edgeId : set->pointedBy())
    {
        Hypergraph::destroy(edgeId);
    }

    // ... finally, destroy the set
    Hypergraph::destroy(id);
}

bool SetSystem::isSet(const unsigned id)
{
    // Transitive closure based
    auto queryId = relatedTo(id, Relation::isALabel);
    auto others = Hypergraph::get(queryId)->pointingTo(this, Set::superclassLabel);
    // NOTE: If we do not delete the result afterwards, the system explodes!
    Hypergraph::destroy(queryId);
    if (others.size())
        return true;
    return false;
}

Set* SetSystem::get(const unsigned id)
{
    return isSet(id) ? static_cast<Set*>(Hypergraph::get(id)) : NULL;
}

Set::Sets SetSystem::find(const std::string& label)
{
    // First call Hypergraph::find to preselect by label, then you should check if each is a set or not and then return the result :)
    Sets result;
    Hyperedge::Hyperedges byLabelIds = Hypergraph::find(label);
    for (auto id : byLabelIds)
    {
        // Check if it is a set
        if (isSet(id))
            result.insert(id);
    }
    return result;
}

/*Relate sets*/
unsigned SetSystem::relateTo(const unsigned idA, const unsigned idB, const std::string& relation) // generic relation "A <label> B"
{
    // ATTENTION: When we call relateTo we might to CREATE sets via isA, so we cannot use Set::get()!
    auto setA = Hypergraph::get(idA);
    auto setB = Hypergraph::get(idB);

    if (!setA || !setB)
        return 0;

    // In any case there will be at least one new hyperedge
    auto relId = Hypergraph::create(relation);
    // Check if there exists already a direct relation with that label between A and B
    // TODO: Should we make a graph traversal here?
    auto hIds = setA->pointingTo(this, relation);
    if (hIds.size())
    {
        // Relation(s) exist
        // We merge them into one new
        // NOTE: Just reusing them is not possbile. Consider this:
        // A -- isA --> C  +  A -- isA --> D 
        // B ----|
        // Then we cannot just add D to the to set of the previous isA. B would incorrectly be a D as well afterwards.
        // What we do now is creating a new isA like this:
        // A -- isA' --> C
        //       |-----> D
        // TODO: The old isA also exists but could be disconnected from A
        for (auto otherId : hIds)
        {
            auto nextId = Hypergraph::unite(relId, otherId);
            // now previous relId is not needed anymore
            Hypergraph::destroy(relId);
            // set the nextId as relId
            relId = nextId;
        }
    }
    // Connect idA --> relId --> idB
    Hypergraph::fromTo(idA, relId);
    Hypergraph::fromTo(relId, idB);
    return relId;
}

// Transitive closures of ...
unsigned SetSystem::relatedTo(const unsigned id, const std::string& relation)
{
    Relation *query;
    // Here we make a graph traversal starting from id
    // every hyperedge X we discover on the way which is reachable from id via
    // relation edges is returned
    // id -- {relation -- *}^N -- relation --> X
    // So, if A -- isA --> B -- isA --> C then afterwards A -- isA --> B, C
    // We have to make sure, that we only consider full triples!!! x -- isA --> y and not only isA --> y
    bool expectRelation = false;
    query = Relation::promote(Hypergraph::get(traversal(
        id,
        [&](Hyperedge *x)
        {
            if (expectRelation)
            {
                // Expecting relation
                expectRelation = false;
                return false;
            } else {
                // Expecting set
                expectRelation = true;
                return ((x->id() != id) && (x->label() != relation)) ? true : false;
            }
        },
        [&](Hyperedge *x, Hyperedge *y){
            if (expectRelation)
            {
                // y should have label of relation
                if (y->label() == relation) return true;
            } else {
                // x should have label of relation
                if (x->label() == relation) return true;
            }
            return false;
        },
        relation,
        DOWN
    )));
    // The created query is a new relation of the same type
    // Therefore id should point to this relation as well
    // ATTENTION: To be usable here, we cannot use the normal Relation::from() here!
    Hypergraph::get(id)->pointTo(this, query->id());
    return query->id();
}

unsigned SetSystem::memberOf(const unsigned id)
{
    return relatedTo(id, Relation::memberOfLabel);
}

unsigned SetSystem::isA(const unsigned id)
{
    return relatedTo(id, Relation::isALabel);
}

unsigned SetSystem::partOf(const unsigned id)
{
    return relatedTo(id, Relation::partOfLabel);
}

unsigned SetSystem::hasA(const unsigned id)
{
    return relatedTo(id, Relation::hasLabel);
}


// Inverse transitive closures of
unsigned SetSystem::relatedToInverse(const unsigned id, const std::string& relation, const std::string& inverse)
{
    Relation *query;
    // Here we make a graph traversal starting from id
    // every hyperedge X we discover on the way which is reachable from id via
    // relation edges is returned
    // id <-- {relation -- *}^N <-- relation -- X
    // NOTE: In contrast to the relatedTo traversal, the direction is reversed!
    // Afterwards the INVERSE LABEL is given to the new relation.
    // So, if A -- isA --> B -- isA --> C then afterwards C -- superclassOf --> B, A (we started at C!)
    bool expectRelation = false;
    query = Relation::promote(Hypergraph::get(traversal(
        id,
        [&](Hyperedge *x)
        {
            if (expectRelation)
            {
                // Expecting relation
                expectRelation = false;
                return false;
            } else {
                // Expecting set
                expectRelation = true;
                return ((x->id() != id) && (x->label() != relation)) ? true : false;
            }
        },
        [&](Hyperedge *x, Hyperedge *y){
            if (expectRelation)
            {
                // y should have label of relation
                if (y->label() == relation) return true;
            } else {
                // x should have label of relation
                if (x->label() == relation) return true;
            }
            return false;
        },
        inverse,
        UP
    )));
    // The created query is a new relation of the same type
    // Therefore id should point to this relation as well
    // ATTENTION: To be usable here, we cannot use the normal Relation::from() here!
    Hypergraph::get(id)->pointTo(this, query->id());
    return query->id();
}

unsigned SetSystem::setOf(const unsigned id)
{
    return relatedToInverse(id, Relation::memberOfLabel, Relation::inverseMemberOfLabel);
}

unsigned SetSystem::superclassOf(const unsigned id)
{
    return relatedToInverse(id, Relation::isALabel, Relation::inverseIsALabel);
}

unsigned SetSystem::wholeOf(const unsigned id)
{
    return relatedToInverse(id, Relation::partOfLabel, Relation::inversePartOfLabel);
}

unsigned SetSystem::ownerOf(const unsigned id)
{
    return relatedToInverse(id, Relation::hasLabel, Relation::inverseHasLabel);
}


// Merge operations on Sets
unsigned SetSystem::unite(const unsigned idA, const unsigned idB)
{
   return 0;
}

unsigned SetSystem::intersect(const unsigned idA, const unsigned idB)
{
   return 0;
}

unsigned SetSystem::subtract(const unsigned idA, const unsigned idB)
{
   return 0;
}
