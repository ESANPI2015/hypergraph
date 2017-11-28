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

Conceptgraph::Conceptgraph(Hypergraph& A)
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

// FIXME: This function should rather be one which uses the UID as the label and not vice versa!!!!
//Hyperedges Conceptgraph::create(const std::string& label)
//{
//    unsigned occurrence = 1;
//    UniqueId id = label;
//    while (create(id, label).empty()) {
//        std::stringstream ss;
//        ss << occurrence;
//        id = label + ss.str();
//        occurrence++;
//    }
//    return Hyperedges{id};
//}

Hyperedges Conceptgraph::find(const std::string& label)
{
    // Find edges which have the right label and are part of the concepts set
    Hyperedges resultIds;
    Hyperedges candidateIds = Hypergraph::get(Conceptgraph::IsConceptId)->pointingFrom();
    for (auto candidateId : candidateIds)
    {
        if (label.empty() || (Hypergraph::get(candidateId)->label() == label))
        {
            resultIds.insert(candidateId);
        }
    }
    return resultIds;
}

Hyperedges Conceptgraph::relations(const std::string& label)
{
    // Find edges which have the right label and are part of the relations set
    Hyperedges resultIds;
    Hyperedges candidateIds = Hypergraph::get(Conceptgraph::IsRelationId)->pointingFrom();
    for (auto candidateId : candidateIds)
    {
        if (label.empty() || (Hypergraph::get(candidateId)->label() == label))
        {
            resultIds.insert(candidateId);
        }
    }
    return resultIds;
}

Hyperedges Conceptgraph::relate(const UniqueId id, const Hyperedges& fromIds, const Hyperedges& toIds, const std::string& label)
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

Hyperedges Conceptgraph::relateFrom(const UniqueId id, const Hyperedges& fromIds, const Hyperedges& toIds, const UniqueId relId)
{
   std::string label = Hypergraph::get(relId)->label();
   return Conceptgraph::relate(id,fromIds,toIds,label);
}

Hyperedges Conceptgraph::relate(const Hyperedges& fromIds, const Hyperedges& toIds, const std::string& label)
{
    unsigned occurrence = 1;
    UniqueId id = label;
    while (Conceptgraph::relate(id, fromIds, toIds, label).empty()) {
        std::stringstream ss;
        ss << occurrence;
        id = label + ss.str();
        occurrence++;
    }
    return unite(Hyperedges{id}, unite(fromIds, toIds));
}

Hyperedges Conceptgraph::relateFrom(const Hyperedges& fromIds, const Hyperedges& toIds, const UniqueId relId)
{
    std::string label = Hypergraph::get(relId)->label();
    unsigned occurrence = 1;
    UniqueId id = relId; // Take relId as basis!!!
    while (Conceptgraph::relate(id, fromIds, toIds, label).empty()) {
        std::stringstream ss;
        ss << occurrence;
        id = label + ss.str();
        occurrence++;
    }
    return unite(Hyperedges{id}, unite(fromIds, toIds));
}

void     Conceptgraph::destroy(const UniqueId id)
{
    if (Hypergraph::get(Conceptgraph::IsConceptId)->isPointingFrom(id))
    {
        // For a concept, we have to get rid of ALL associated relations (EXCEPT the urrelation)
        auto relationIds = subtract(relationsOf(Hyperedges{id}), Hyperedges{Conceptgraph::IsConceptId});
        for (auto relId : relationIds)
        {
            Hypergraph::destroy(relId);
        }
    }
    // Removing a relation does not imply anything than just calling the base class destroy thing, right?
    // NOTE: That we allow relations also being concepts for now.
    Hypergraph::destroy(id);
}

Hyperedges Conceptgraph::relationsFrom(const Hyperedges& ids, const std::string& label)
{
    Hyperedges resultIds;
    // Find edges which have the right label and are part of the relation set
    Hyperedges candidateIds = Hypergraph::get(Conceptgraph::IsRelationId)->pointingFrom();
    for (auto candidateId : candidateIds)
    {
        auto rel = Hypergraph::get(candidateId);
        // Valid edge?
        if (!rel)
            continue;
        // Match by label?
        if (!label.empty() && (rel->label() != label))
            continue;
        // Found a relation with the given label
        for (auto id : ids)
        {
            // Now we have to check if conceptId is part of the from set
            if (rel->isPointingFrom(id))
            {
                // gotya!
                resultIds.insert(candidateId);
            }
        }
    }
    return resultIds;
}

Hyperedges Conceptgraph::relationsTo(const Hyperedges& ids, const std::string& label)
{
    Hyperedges resultIds;
    // Find edges which have the right label and are part of the relation set
    Hyperedges candidateIds = Hypergraph::get(Conceptgraph::IsRelationId)->pointingFrom();
    for (auto candidateId : candidateIds)
    {
        auto rel = Hypergraph::get(candidateId);
        // Valid edge?
        if (!rel)
            continue;
        // Match by label?
        if (!label.empty() && (rel->label() != label))
            continue;
        // Found a relation with the given label
        for (auto id : ids)
        {
            // Now we have to check if conceptId is part of the to set
            if (rel->isPointingTo(id))
            {
                // gotya!
                resultIds.insert(candidateId);
            }
        }
    }
    return resultIds;
}

Hyperedges Conceptgraph::traverse(const UniqueId rootId,
                    const std::vector<std::string>& visitLabels,
                    const std::vector<std::string>& relationLabels,
                    const TraversalDirection dir)
{
    Hyperedges result;
    Hyperedges visited;
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
            result.insert(current->id());
        }

        // TODO: Rewrite this part using relationsTo and relationsFrom depending on dir!
        // Get all relations which match at least one of the relationLabels
        Hyperedges relations;
        for (std::string label : relationLabels)
        {
            Hyperedges some = relationsOf(Hyperedges{current->id()}, label);
            relations.insert(some.begin(), some.end());
        }

        for (auto relId : relations)
        {
            // Put all successor hedges (DOWN) or predecessor hedges (UP)  into the set of toVisit to be searched
            auto rel = get(relId);
            switch (dir)
            {
                case DOWN:
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
                case UP:
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

Hyperedges Conceptgraph::traverse(const UniqueId rootId, const std::string& visitLabel, const std::string& relationLabel, const TraversalDirection dir)
{
    std::vector<std::string> v;
    std::vector<std::string> r;
    if (!visitLabel.empty())
        v.push_back(visitLabel);
    if (!relationLabel.empty())
        r.push_back(relationLabel);
    return traverse(rootId, v, r, dir);
}
