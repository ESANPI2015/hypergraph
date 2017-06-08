#include "Conceptgraph.hpp"

const std::string Conceptgraph::ConceptLabel = "CONCEPT";
const std::string Conceptgraph::RelationLabel = "RELATION";

Conceptgraph::Conceptgraph()
: Hypergraph()
{
}

Conceptgraph::Conceptgraph(Hypergraph& A)
{
    // Construct from a given hypergraph
    // This means cloning all hyperedges and sorting them into _concepts or _relations sets
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
    Hypergraph::from(fromId, id); // FIXME: Can fail
    Hypergraph::to(id, toId);     // FIXME: Can fail
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

unsigned   Conceptgraph::transitiveClosure(const unsigned root, const std::string& relationLabel)
{
    return 0;
}
