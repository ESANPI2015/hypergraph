#include "Conceptgraph.hpp"
#include <map>
#include <functional>
#include <sstream>
#include <algorithm>

const UniqueId Conceptgraph::IsConceptId = "Conceptgraph::IsConceptId";
const UniqueId Conceptgraph::IsRelationId = "Conceptgraph::IsRelationId";

Conceptgraph::Conceptgraph()
: Hypergraph()
{
    createFundamentals();
}

Conceptgraph::Conceptgraph(const Hypergraph& A)
: Hypergraph(A)
{
    createFundamentals();
}

void Conceptgraph::createFundamentals()
{
    // Create the URHEDGES (if they exist, nothing will happen :))
    // The following is the fundametal model:
    // C <- IS-CONCEPT <- IS-RELATION
    //               R <-----|
    Hypergraph::create(Conceptgraph::IsRelationId, "IS-RELATION");
    if (!exists(Conceptgraph::IsConceptId))
    {
        Hypergraph::create(Conceptgraph::IsConceptId, "IS-CONCEPT");
        Hypergraph::from(Hyperedges{Conceptgraph::IsConceptId}, Hyperedges{Conceptgraph::IsRelationId});
    }
}

Hyperedges Conceptgraph::create(const UniqueId& id, const std::string& label)
{
    if (!Hypergraph::create(id, label).empty())
    {
        Hypergraph::from(Hyperedges{id}, Hyperedges{Conceptgraph::IsConceptId}); // This cannot fail
        return Hyperedges{id};
    }
    return Hyperedges();
}

Hyperedges Conceptgraph::find(const std::string& label) const
{
    // Find edges which have the right label and are part of the concepts set
    return Hypergraph::from(Hyperedges{Conceptgraph::IsConceptId}, label);
}

Hyperedges Conceptgraph::relations(const std::string& label) const
{
    // Find edges which have the right label and are part of the relations set
    return Hypergraph::from(Hyperedges{Conceptgraph::IsRelationId}, label);
}

Hyperedges Conceptgraph::relate(const UniqueId& id, const Hyperedges& fromIds, const Hyperedges& toIds, const std::string& label)
{
    // Creating relations means creating a (X <- IS-RELATION) pair
    if (Hypergraph::create(id, label).empty())
        return Hyperedges();
    Hypergraph::from(Hyperedges{id}, Hyperedges{Conceptgraph::IsRelationId});

    // Furthermore, we have to connect the relation
    // NOTE: relations can also relate relations not only concepts!!!
    Hypergraph::from(fromIds, Hyperedges{id});
    Hypergraph::to(Hyperedges{id}, toIds);
    return Hyperedges{id};
}

Hyperedges Conceptgraph::relateFrom(const UniqueId& id, const Hyperedges& fromIds, const Hyperedges& toIds, const UniqueId& relId)
{
   const std::string& label(Hypergraph::read(relId).label());
   // NOTE: Even though we allow the in- and outdegree of a relation created from a template to mismatch, we should check them afterwards to ensure correct arity.
   return Conceptgraph::relate(id,fromIds,toIds,label);
}

Hyperedges Conceptgraph::relate(const Hyperedges& fromIds, const Hyperedges& toIds, const std::string& label)
{
    // See also https://en.cppreference.com/w/cpp/utility/hash
    UniqueId id(label);
    for (const UniqueId& saltId : unite(fromIds, toIds))
    {
        auto myHash(std::hash<UniqueId>{}(id));
        auto newHash(std::hash<UniqueId>{}(saltId));
        id = std::to_string(myHash ^ (newHash << 1));
    }
    // Re-hash in case of an (unlikely) collision
    while (Conceptgraph::relate(id, fromIds, toIds, label).empty()) {
        auto myHash(std::hash<UniqueId>{}(id));
        auto newHash(std::hash<UniqueId>{}(label));
        id = std::to_string(myHash ^ (newHash << 1));
    }
    return Hyperedges{id};
}

Hyperedges Conceptgraph::relateFrom(const Hyperedges& fromIds, const Hyperedges& toIds, const UniqueId& relId)
{
    // See also https://en.cppreference.com/w/cpp/utility/hash
    UniqueId id(relId);
    for (const UniqueId& saltId : unite(fromIds, toIds))
    {
        auto myHash(std::hash<UniqueId>{}(id));
        auto newHash(std::hash<UniqueId>{}(saltId));
        id = std::to_string(myHash ^ (newHash << 1));
    }
    const std::string& label(Hypergraph::read(relId).label());
    // Re-hash in case of an (unlikely) collision
    while (Conceptgraph::relateFrom(id, fromIds, toIds, relId).empty()) {
        auto myHash(std::hash<UniqueId>{}(id));
        auto newHash(std::hash<UniqueId>{}(label));
        id = std::to_string(myHash ^ (newHash << 1));
    }
    return Hyperedges{id};
}

void     Conceptgraph::destroy(const UniqueId& id)
{
    // Avoid bad ids
    if (!exists(id))
        return;

    // Very important: We should never delete our two BASIC RELATIONS
    if (id == Conceptgraph::IsConceptId)
        return;
    if (id == Conceptgraph::IsRelationId)
        return;

    // Before we can destroy any relation, we have to check all other relations referring to us
    std::set< UniqueId > toBeDestroyed;
    // Mark all relations pointingFrom us to be deleted iff they do not point from something else
    Hyperedges relsFromUs(relationsFrom(Hyperedges{id}));
    for (const UniqueId& relId : relsFromUs)
    {
        // Avoid bad ids
        // TODO: Check if necessary
        if (!exists(relId))
            continue;
        // If it is not pointing from anything anymore, we destroy it
        if (Hypergraph::read(relId).pointingFrom().size() <= 1)
            toBeDestroyed.insert(relId);
    }
    // Mark all relations pointingTo us to be deleted iff they do not point to something else
    Hyperedges relsToUs(relationsTo(Hyperedges{id}));
    for (const UniqueId& relId : relsToUs)
    {
        // Avoid bad ids
        // TODO: Check if necessary
        if (!exists(relId))
            continue;
        // If it is not pointing to anything anymore, we destroy it
        if (Hypergraph::read(relId).pointingTo().size() <= 1)
            toBeDestroyed.insert(relId);
    }
    // Destroy all marked relations mentioned (NOTE: We call Conceptgraph::destroy here to destroy the relation chains!)
    for (const UniqueId& relId : toBeDestroyed)
    {
        Conceptgraph::destroy(relId);
    }

    // Finally we destroy the Hyperedge itself
    Hypergraph::destroy(id);
}

Hyperedges Conceptgraph::relationsFrom(const Hyperedges& ids, const std::string& label) const
{
    // All relations with a certain label
    Hyperedges all(relations(label));
    // All hyperedges pointing from us
    Hyperedges pointingFromUs;
    for (const UniqueId& id : ids)
    {
        Hyperedges cache(read(id)._fromOthers);
        pointingFromUs = unite(pointingFromUs, cache);
    }
    // All relations with a certain label pointing from us
    return intersect(all, pointingFromUs);
}

Hyperedges Conceptgraph::relationsTo(const Hyperedges& ids, const std::string& label) const
{
    // All relations with a certain label
    Hyperedges all(relations(label));
    // All hyperedges pointing to us
    Hyperedges pointingToUs;
    for (const UniqueId& id : ids)
    {
        Hyperedges cache(read(id)._toOthers);
        pointingToUs = unite(pointingToUs, cache);
    }
    // All relations with a certain label pointing to us
    return intersect(all, pointingToUs);
}
