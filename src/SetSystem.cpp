#include "SetSystem.hpp"
#include "Set.hpp"
#include "Relation.hpp"
#include <iostream>

SetSystem::SetSystem()
{
    // Create the superclass and remember its id
    // NOTE: The superclass cannot be created with the SetSystem create (otherwise it would get an isA relation to nowhere?)
    superclassId = Hypergraph::create(Set::superclassLabel);
}

SetSystem::~SetSystem()
{
    // Do we have to do something here?
}

/*Factory functions*/
unsigned SetSystem::create(const std::string& label)
{
    Set *neu = new Set(getNextId(), label);
    _edges[neu->id()] = neu;
    relateTo(neu->id(), superclassId, Relation::isALabel);
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
    Set* neu = get(id);
    if (!neu)
    {
        // Create a new hyperedge
        // Give it the desired id
        neu = new Set(id, label);
        _edges[id] = neu;
        relateTo(neu->id(), superclassId, Relation::isALabel);
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
    // ATTENTION: is called by Set::get! So we cannot use Set::get here as well!
    // Therefore we also cannot use the transitive closure here ... because it uses relation functions
    // Here: if any of the direct relations labelled isA is pointing to the superclass we are happy :)
    auto edge = Hypergraph::get(id);
    if (!edge)
        return false;
    auto rels = edge->pointingTo(this, Relation::isALabel);
    for (auto relId : rels)
    {
        auto rel = Hypergraph::get(relId);
        // FIXME: Either check for label or for superclassId
        if (rel->pointingTo().count(superclassId))
            return true;
    }
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
    query = Relation::promote(Hypergraph::get(traversal(
        id,
        [&](Hyperedge *x)
        {
            return ((x->id() != id) && (x->label() != relation)) ? true : false;
        },
        [&](Hyperedge *x, Hyperedge *y){
            return ((x->label() == relation) || (y->label() == relation)) ? true : false;
        },
        relation,
        DOWN
    )));
    // The created query is a new relation of the same type
    // Therefore id should point to this relation as well
    query->from(this,id);
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
    query = Relation::promote(Hypergraph::get(traversal(
        id,
        [&](Hyperedge *x)
        {
            return ((x->id() != id) && (x->label() != relation)) ? true : false;
        },
        [&](Hyperedge *x, Hyperedge *y){
            return ((x->label() == relation) || (y->label() == relation)) ? true : false;
        },
        inverse,
        UP
    )));
    // The created query is a new relation of the same type
    // Therefore id should point to this relation as well
    query->from(this, id);
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
