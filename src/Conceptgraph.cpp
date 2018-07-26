#include "Conceptgraph.hpp"
#include <map>
#include <set>
#include <queue>
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
    Hypergraph::create(Conceptgraph::IsConceptId, "IS-CONCEPT");
    Hypergraph::create(Conceptgraph::IsRelationId, "IS-RELATION");
    Hypergraph::from(Hyperedges{Conceptgraph::IsConceptId}, Hyperedges{Conceptgraph::IsRelationId});
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

Hyperedges Conceptgraph::find(const std::string& label)
{
    // Find edges which have the right label and are part of the concepts set
    return Hypergraph::from(Hyperedges{Conceptgraph::IsConceptId}, label);
}

Hyperedges Conceptgraph::relations(const std::string& label)
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
    return unite(Hyperedges{id}, unite(fromIds, toIds));
}

Hyperedges Conceptgraph::relateFrom(const UniqueId& id, const Hyperedges& fromIds, const Hyperedges& toIds, const UniqueId& relId)
{
   std::string label = Hypergraph::get(relId)->label();
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
    return unite(Hyperedges{id}, unite(fromIds, toIds));
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
    const std::string& label(Hypergraph::get(relId)->label());
    // Re-hash in case of an (unlikely) collision
    while (Conceptgraph::relate(id, fromIds, toIds, label).empty()) {
        auto myHash(std::hash<UniqueId>{}(id));
        auto newHash(std::hash<UniqueId>{}(label));
        id = std::to_string(myHash ^ (newHash << 1));
    }
    return unite(Hyperedges{id}, unite(fromIds, toIds));
}

void     Conceptgraph::destroy(const UniqueId& id)
{
    // Very important: We should never delete our two BASIC RELATIONS
    if (id == Conceptgraph::IsConceptId)
        return;
    if (id == Conceptgraph::IsRelationId)
        return;
    if (Hypergraph::get(Conceptgraph::IsConceptId)->isPointingFrom(id))
    {
        // When we delete a concept we check if
        // a) all relations pointingFrom us still point from something else
        Hyperedges relsFromUs(relationsFrom(Hyperedges{id}));
        for (UniqueId relId : relsFromUs)
        {
            // Ignore URRELATION
            if (relId == Conceptgraph::IsConceptId)
                continue;
            // If it is not pointing from anything anymore, we destroy it
            if (Hypergraph::get(relId)->pointingFrom().size() <= 1)
                Hypergraph::destroy(relId);
        }
        // b) all relations pointingTo us still point to something else
        Hyperedges relsToUs(relationsTo(Hyperedges{id}));
        for (UniqueId relId : relsToUs)
        {
            // If it is not pointing to anything anymore, we destroy it
            if (Hypergraph::get(relId)->pointingTo().size() <= 1)
                Hypergraph::destroy(relId);
        }
    }
    // Removing a relation does not imply anything than just calling the base class destroy thing, right?
    // NOTE: That we allow relations also being concepts for now.
    Hypergraph::destroy(id);
}

Hyperedges Conceptgraph::relationsFrom(const Hyperedges& ids, const std::string& label)
{
    // All relations with a certain label
    Hyperedges all(relations(label));
    // All hyperedges pointing from us
    Hyperedges pointingFromUs;
    for (const UniqueId& id : ids)
    {
        Hyperedges cache(get(id)->_fromOthers);
        pointingFromUs = unite(pointingFromUs, cache);
    }
    // All relations with a certain label pointing from us
    return intersect(all, pointingFromUs);
}

Hyperedges Conceptgraph::relationsTo(const Hyperedges& ids, const std::string& label)
{
    // All relations with a certain label
    Hyperedges all(relations(label));
    // All hyperedges pointing to us
    Hyperedges pointingToUs;
    for (const UniqueId& id : ids)
    {
        Hyperedges cache(get(id)->_toOthers);
        pointingToUs = unite(pointingToUs, cache);
    }
    // All relations with a certain label pointing to us
    return intersect(all, pointingToUs);
}

Hyperedges Conceptgraph::traverse(const UniqueId& rootId,
                    const std::vector<std::string>& visitLabels,
                    const std::vector<std::string>& relationLabels,
                    const TraversalDirection dir)
{
    Hyperedges result;
    std::set< UniqueId > visited;
    std::queue< UniqueId > toVisit;

    toVisit.push(rootId);

    // Run through queue of unknown edges
    while (!toVisit.empty())
    {
        auto current = get(toVisit.front());
        toVisit.pop();

        if (visited.count(current->id()))
            continue;

        // Visiting!!!
        visited.insert(current->id());
        // Insert the hedge iff either visitLabels is empty OR current label matches one of the visitLabels
        if (!visitLabels.size() || (std::find(visitLabels.begin(), visitLabels.end(), current->label()) != visitLabels.end()))
        {
            // edge matches filter
            result.push_back(current->id());
        }

        // TODO: Rewrite this part using relationsTo and relationsFrom depending on dir!
        // Get all relations which match at least one of the relationLabels
        Hyperedges relations;
        for (std::string label : relationLabels)
        {
            Hyperedges some = relationsOf(Hyperedges{current->id()}, label);
            relations.insert(relations.end(), some.begin(), some.end());
        }

        for (auto relId : relations)
        {
            // Put all successor hedges (FORWARD) or predecessor hedges (INVERSE)  into the set of toVisit to be searched
            auto rel = get(relId);
            switch (dir)
            {
                case FORWARD:
                    if (rel->isPointingFrom(current->id()))
                    {
                        auto others = rel->pointingTo();
                        for (auto otherId : others)
                        {
                            toVisit.push(otherId);
                        }
                    }
                    break;
                case BOTH:
                    if (rel->isPointingFrom(current->id()))
                    {
                        auto others = rel->pointingTo();
                        for (auto otherId : others)
                        {
                            toVisit.push(otherId);
                        }
                    }
                case INVERSE:
                    if (rel->isPointingTo(current->id()))
                    {
                        auto others = rel->pointingFrom();
                        for (auto otherId : others)
                        {
                            toVisit.push(otherId);
                        }
                    }
                    break;
            }
        }
    }

    return result;
}

Hyperedges Conceptgraph::traverse(const UniqueId& rootId, const std::string& visitLabel, const std::string& relationLabel, const TraversalDirection dir)
{
    std::vector<std::string> v;
    std::vector<std::string> r;
    if (!visitLabel.empty())
        v.push_back(visitLabel);
    if (!relationLabel.empty())
        r.push_back(relationLabel);
    return traverse(rootId, v, r, dir);
}
