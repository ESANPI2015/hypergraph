#include "Conceptgraph.hpp"
#include <map>

const std::string Conceptgraph::ConceptLabel = "CONCEPT";
const std::string Conceptgraph::RelationLabel = "RELATION";

Conceptgraph::Conceptgraph()
: Hypergraph()
{
}

Conceptgraph::Conceptgraph(Hypergraph& A)
{
    // Construct from a given hypergraph
    // Get the master edges first
    Hyperedges labelledAsConcept = A.find(Conceptgraph::ConceptLabel);
    Hyperedges labelledAsRelation = A.find(Conceptgraph::RelationLabel);
    // Parse concepts
    Hyperedges allConcepts;
    for (auto masterId : labelledAsConcept)
    {
        auto concepts = A.get(masterId)->pointingTo();
        allConcepts.insert(concepts.begin(), concepts.end());
    }
    // Parse relations
    Hyperedges allRelations;
    for (auto masterId : labelledAsRelation)
    {
        auto relations = A.get(masterId)->pointingTo();
        allRelations.insert(relations.begin(), relations.end());
    }

    // So, create all concepts first (with possibly new ids)
    std::map<unsigned, unsigned> old2newIds;
    for (auto conceptId : allConcepts)
    {
        auto concept = A.get(conceptId);
        auto id = create(concept->label());
        old2newIds[conceptId] = id;
    }
    // ... then we create the relations
    for (auto relationId : allRelations)
    {
        // NOTE: We use relate here, but it will fail to wire things (imagine relations of relations)
        auto relation = A.get(relationId);
        auto id = relate(0,0,relation->label());
        old2newIds[relationId] = id;
    }
    // Wire all relations
    for (auto relationId : allRelations)
    {
        auto newRelId = old2newIds[relationId];
        for (auto otherId : A.get(relationId)->pointingTo())
        {
            auto newOtherId = old2newIds[otherId];
            to(newRelId, newOtherId);
        }
        for (auto otherId : A.get(relationId)->pointingFrom())
        {
            auto newOtherId = old2newIds[otherId];
            from(newOtherId, newRelId);
        }
    }
}

unsigned Conceptgraph::getConceptHyperedge()
{
    unsigned id;
    Hyperedges candidateIds = Hypergraph::find(Conceptgraph::ConceptLabel);
    if (candidateIds.size())
    {
        // found, so choose one
        id = *candidateIds.begin();
    } else {
        // none found, so create hyperedge
        id = Hypergraph::create(Conceptgraph::ConceptLabel);
    }
    return id;
}

unsigned Conceptgraph::getRelationHyperedge()
{
    unsigned id;
    Hyperedges candidateIds = Hypergraph::find(Conceptgraph::RelationLabel);
    if (candidateIds.size())
    {
        // found, so choose one
        id = *candidateIds.begin();
    } else {
        // none found, so create hyperedge
        id = Hypergraph::create(Conceptgraph::RelationLabel);
    }
    return id;
}

unsigned Conceptgraph::create(const std::string& label)
{
    // Creating a concepts means creating a (CONCEPT -> X) pair if a CONCEPT hyperedge does not yet exist
    unsigned id = Hypergraph::create(label);
    unsigned conceptId = getConceptHyperedge();
    Hypergraph::to(conceptId, id); // This cannot fail
    // We could either add this new edge to the set of _concepts or call a reparse() function
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
    unsigned relId = getRelationHyperedge();
    Hypergraph::to(relId, id); // This cannot fail
    // Furthermore, we have to connect the relation
    // NOTE: relations can also relate relations not only concepts!!!
    Hypergraph::from(fromId, id); // FIXME: Can fail
    Hypergraph::to(id, toId);     // FIXME: Can fail
    // We could either add this new edge to the set of _relations or call a reparse() function
    _relations.insert(id);
    return id;
}

unsigned Conceptgraph::relate(const Hyperedges& fromIds, const Hyperedges& toIds, const std::string& label)
{
    // Creating relations means creating a (RELATION -> X) pair
    unsigned id = Hypergraph::create(label);
    unsigned relId = getRelationHyperedge();
    Hypergraph::to(relId, id); // This cannot fail
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
