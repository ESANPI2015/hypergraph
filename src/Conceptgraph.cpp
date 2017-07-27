#include "Conceptgraph.hpp"
#include <map>
#include <set>
#include <queue>
#include <functional>
#include <sstream>
#include <algorithm>

const unsigned Conceptgraph::ConceptId = 1;
const unsigned Conceptgraph::RelationId = 2;

Conceptgraph::Conceptgraph()
: Hypergraph()
{
    // Create the URHEDGES (if they exist, nothing will happen :))
    Hypergraph::create(Conceptgraph::ConceptId, "CONCEPT");
    Hypergraph::create(Conceptgraph::RelationId, "RELATION");
}

Conceptgraph::Conceptgraph(Hypergraph& A)
: Hypergraph(A)
{
    // Create the URHEDGES (if they exist, nothing will happen :))
    Hypergraph::create(Conceptgraph::ConceptId, "CONCEPT");
    Hypergraph::create(Conceptgraph::RelationId, "RELATION");
}

bool Conceptgraph::create(const unsigned id, const std::string& label)
{
    if (Hypergraph::create(id, label))
    {
        Hypergraph::create(Conceptgraph::ConceptId, "CONCEPT");
        Hypergraph::to(Conceptgraph::ConceptId, id); // This cannot fail
        return true;
    }
    return false;
}

unsigned Conceptgraph::create(const std::string& label)
{
    unsigned id = std::hash<std::string>{}(label);
    if (create(id, label))
        return id;
    return 0;
}

Hypergraph::Hyperedges Conceptgraph::find(const std::string& label)
{
    // Find edges which have the right label and are part of the concepts set
    Hyperedges resultIds;
    Hyperedges candidateIds = Hypergraph::get(Conceptgraph::ConceptId)->pointingTo();
    for (auto candidateId : candidateIds)
    {
        if (label.empty() || (Hypergraph::get(candidateId)->label() == label))
        {
            resultIds.insert(candidateId);
        }
    }
    return resultIds;
}

Hypergraph::Hyperedges Conceptgraph::relations(const std::string& label)
{
    // Find edges which have the right label and are part of the relations set
    Hyperedges resultIds;
    Hyperedges candidateIds = Hypergraph::get(Conceptgraph::RelationId)->pointingTo();
    for (auto candidateId : candidateIds)
    {
        if (label.empty() || (Hypergraph::get(candidateId)->label() == label))
        {
            resultIds.insert(candidateId);
        }
    }
    return resultIds;
}

bool Conceptgraph::relate(const unsigned id, const unsigned fromId, const unsigned toId, const std::string& label)
{
    // Creating relations means creating a (RELATION -> X) pair
    if (!Hypergraph::create(id, label))
        return false;
    Hypergraph::create(Conceptgraph::RelationId, "RELATION");
    Hypergraph::to(Conceptgraph::RelationId, id);

    // Furthermore, we have to connect the relation
    // NOTE: relations can also relate relations not only concepts!!!
    Hypergraph::from(fromId, id);
    Hypergraph::to(id, toId);
    return true;
}

bool Conceptgraph::relate(const unsigned id, const unsigned fromId, const unsigned toId, const unsigned relId)
{
   std::string label = Hypergraph::get(relId)->label();
   return Conceptgraph::relate(id,fromId,toId,label);
}

bool Conceptgraph::relate(const unsigned id, const Hyperedges& fromIds, const Hyperedges& toIds, const std::string& label)
{
    // Creating relations means creating a (RELATION -> X) pair
    if (!Hypergraph::create(id, label))
        return false;
    Hypergraph::create(Conceptgraph::RelationId, "RELATION");
    Hypergraph::to(Conceptgraph::RelationId, id);

    // Furthermore, we have to connect the relation
    // NOTE: relations can also relate relations not only concepts!!!
    Hypergraph::from(fromIds, id);
    Hypergraph::to(id, toIds);
    return true;
}

bool Conceptgraph::relate(const unsigned id, const Hyperedges& fromIds, const Hyperedges& toIds, const unsigned relId)
{
   std::string label = Hypergraph::get(relId)->label();
   return Conceptgraph::relate(id,fromIds,toIds,label);
}

unsigned Conceptgraph::relate(const unsigned fromId, const unsigned toId, const std::string& label)
{
    std::stringstream ss;
    ss << get(fromId)->label() << get(toId)->label() << label;
    unsigned id = std::hash<std::string>{}(ss.str());
    if (relate(id, fromId, toId, label))
        return id;
    return 0;
}

unsigned Conceptgraph::relate(const unsigned fromId, const unsigned toId, const unsigned relId)
{
   std::string label = Hypergraph::get(relId)->label();
   return Conceptgraph::relate(fromId,toId,label);
}

unsigned Conceptgraph::relate(const Hyperedges& fromIds, const Hyperedges& toIds, const std::string& label)
{
    std::stringstream ss;
    for (auto fromId : fromIds)
        ss << get(fromId)->label();
    for (auto toId : toIds)
        ss << get(toId)->label();
    ss << label;
    unsigned id = std::hash<std::string>{}(ss.str());
    if (relate(id, fromIds, toIds, label))
        return id;
    return 0;
}

unsigned Conceptgraph::relate(const Hyperedges& fromIds, const Hyperedges& toIds, const unsigned relId)
{
   std::string label = Hypergraph::get(relId)->label();
   return Conceptgraph::relate(fromIds,toIds,label);
}

void     Conceptgraph::destroy(const unsigned id)
{
    if (Hypergraph::get(Conceptgraph::ConceptId)->isPointingTo(id))
    {
        // For a concept, we have to get rid of ALL associated relations
        auto relationIds = relationsOf(id);
        for (auto relId : relationIds)
        {
            Hypergraph::destroy(relId);
        }
    }
    // Removing a relation does not imply anything than just calling the base class destroy thing, right?
    // NOTE: That we allow relations also being concepts for now.
    Hypergraph::destroy(id);
}

Hypergraph::Hyperedges Conceptgraph::relationsOf(const unsigned id, const std::string& label)
{
    Hyperedges resultIds;
    // Find edges which have the right label and are part of the relation set
    Hyperedges candidateIds = Hypergraph::get(Conceptgraph::RelationId)->pointingTo();
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
        // Now we have to check if conceptId is part of either the from or the to set
        if (rel->isPointingTo(id) || rel->isPointingFrom(id))
        {
            // gotya!
            resultIds.insert(candidateId);
        }
    }
    return resultIds;
}

Hypergraph::Hyperedges Conceptgraph::relationsOf(const Hyperedges& ids, const std::string& label)
{
    Hyperedges result;
    for (auto id : ids)
    {
        auto relIds = relationsOf(id, label);
        result.insert(relIds.begin(), relIds.end());
    }
    return result;
}

Hypergraph::Hyperedges Conceptgraph::traverse(const unsigned rootId,
                    const std::vector<std::string>& visitLabels,
                    const std::vector<std::string>& relationLabels,
                    const TraversalDirection dir)
{
    Hyperedges result;
    Hyperedges visited;
    std::queue< unsigned > toVisit;

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

        // Get all relations which match at least one of the relationLabels
        Hyperedges relations;
        for (std::string label : relationLabels)
        {
            Hyperedges some = relationsOf(current->id(), label);
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

Hypergraph::Hyperedges Conceptgraph::traverse(const unsigned rootId, const std::string& visitLabel, const std::string& relationLabel, const TraversalDirection dir)
{
    std::vector<std::string> v;
    std::vector<std::string> r;
    if (!visitLabel.empty())
        v.push_back(visitLabel);
    if (!relationLabel.empty())
        r.push_back(relationLabel);
    return traverse(rootId, v, r, dir);
}
