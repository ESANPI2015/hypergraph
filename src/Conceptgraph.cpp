#include "Conceptgraph.hpp"
#include <map>
#include <set>
#include <queue>

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
    // We assume that the Hypergraph contains HEDGES with the specified ids.
    // If it does, we will sort every HEDGE in their TO sets to either CONCEPT or RELATION set
    Hyperedge* masterConcept = Hypergraph::get(Conceptgraph::ConceptId);
    Hyperedge* masterRelation = Hypergraph::get(Conceptgraph::RelationId);
    if (masterConcept)
    {
        Hyperedges allConcepts = masterConcept->pointingTo();
        _concepts.insert(allConcepts.begin(), allConcepts.end());
        if (masterRelation)
        {
            Hyperedges allRelations = masterRelation->pointingTo();
            _relations.insert(allRelations.begin(), allRelations.end());
        }
    }
}

unsigned Conceptgraph::create(const std::string& label)
{
    // Creating a concepts means creating a (HEDGE(ConceptId) -> HEDGE(label)) pair
    unsigned id = Hypergraph::create(label);
    Hypergraph::create(Conceptgraph::ConceptId, "CONCEPT");
    Hypergraph::to(Conceptgraph::ConceptId, id); // This cannot fail
    _concepts.insert(id);
    return id;
}

Hypergraph::Hyperedges Conceptgraph::find(const std::string& label)
{
    // Find edges which have the right label and are part of the _concepts set
    Hyperedges resultIds;
    Hyperedges edgeIds = Hypergraph::find(label);
    for (auto edgeId : edgeIds)
    {
        if (_concepts.count(edgeId))
        {
            resultIds.insert(edgeId);
        }
    }
    return resultIds;
}

unsigned Conceptgraph::relate(const unsigned fromId, const unsigned toId, const std::string& label)
{
    // Creating relations means creating a (RELATION -> X) pair
    unsigned id = Hypergraph::create(label);
    Hypergraph::create(Conceptgraph::RelationId, "RELATION");
    Hypergraph::to(Conceptgraph::RelationId, id); // This cannot fail
    // Furthermore, we have to connect the relation
    // NOTE: relations can also relate relations not only concepts!!!
    Hypergraph::from(fromId, id); // FIXME: Can fail if fromId is not in graph
    Hypergraph::to(id, toId);     // FIXME: Can fail
    // We could either add this new edge to the set of _relations or call a reparse() function
    _relations.insert(id);
    return id;
}

unsigned Conceptgraph::relate(const Hyperedges& fromIds, const Hyperedges& toIds, const std::string& label)
{
    // Creating relations means creating a (RELATION -> X) pair
    unsigned id = Hypergraph::create(label);
    Hypergraph::create(Conceptgraph::RelationId, "RELATION");
    Hypergraph::to(Conceptgraph::RelationId, id); // This cannot fail
    // Furthermore, we have to connect the relation
    // NOTE: relations can also relate relations not only concepts!!!
    Hypergraph::from(fromIds, id); // FIXME: Can fail
    Hypergraph::to(id, toIds);     // FIXME: Can fail
    // We could either add this new edge to the set of _relations or call a reparse() function
    _relations.insert(id);
    return id;
}

void     Conceptgraph::destroy(const unsigned id)
{
    if (_concepts.count(id))
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

Hypergraph::Hyperedges Conceptgraph::relationsOf(const unsigned conceptId, const std::string& relationLabel)
{
    Hyperedges result;
    // First we have to get all hyperedges with a certain label
    Hyperedges allIds = Hypergraph::find(relationLabel);
    for (auto edgeId : allIds)
    {
        // ... then we check if it also is a relation
        if (_relations.count(edgeId))
        {
            // Found a relation with the given label
            // Now we have to check if conceptId is part of either the from or the to set
            auto rel = get(edgeId);
            if (rel && (rel->isPointingTo(conceptId) || rel->isPointingFrom(conceptId)))
            {
                // gotya!
                result.insert(edgeId);
            }
        }
    }
    return result;
}

Hypergraph::Hyperedges Conceptgraph::relationsOf(const Hyperedges& concepts, const std::string& relationLabel)
{
    Hyperedges result;
    for (auto conceptId : concepts)
    {
        auto relIds = relationsOf(conceptId, relationLabel);
        result.insert(relIds.begin(), relIds.end());
    }
    return result;
}

Hypergraph::Hyperedges Conceptgraph::traverse(const unsigned rootId, const std::string& conceptLabel, const std::string& relationLabel, const TraversalDirection dir)
{
    Hyperedges result;
    Hyperedges visited;
    std::queue< unsigned > concepts;

    concepts.push(rootId);

    // Run through queue of unknown edges
    while (!concepts.empty())
    {
        auto concept = get(concepts.front());
        concepts.pop();

        if (visited.count(concept->id()))
            continue;

        // Visiting!!!
        visited.insert(concept->id());
        if (conceptLabel.empty() || (concept->label() == conceptLabel))
        {
            // edge matches filter func
            result.insert(concept->id());
        }

        // Get all relations with relationLabel
        Hyperedges relations = relationsOf(concept->id(), relationLabel);
        for (auto relId : relations)
        {
            // Put all successor hedges (DOWN) or predecessor hedges (UP)  into the set of concepts to be searched
            auto rel = get(relId);
            switch (dir)
            {
                case DOWN:
                    if (rel->isPointingFrom(concept->id()))
                    {
                        auto others = rel->pointingTo();
                        for (auto otherId : others)
                        {
                            concepts.push(otherId);
                        }
                    }
                    break;
                case BOTH:
                    if (rel->isPointingFrom(concept->id()))
                    {
                        auto others = rel->pointingTo();
                        for (auto otherId : others)
                        {
                            concepts.push(otherId);
                        }
                    }
                case UP:
                    if (rel->isPointingTo(concept->id()))
                    {
                        auto others = rel->pointingFrom();
                        for (auto otherId : others)
                        {
                            concepts.push(otherId);
                        }
                    }
                    break;
            }
        }
    }

    return result;
}
