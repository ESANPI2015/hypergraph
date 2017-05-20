#include "Set.hpp"
#include "SetSystem.hpp"

const std::string Set::superclassLabel = "SET";

Set::Set(const unsigned id, const std::string& label)
: Hyperedge(id,label)
{
}

Set::Sets Set::members(SetSystem *sys, const std::string& label)
{
    Set::Sets result;
    Hyperedge::Hyperedges rels = pointedBy(sys, Relation::memberOfLabel); // Gives all memberOf relations pointing to us

    for (auto relId : rels)
    {
        // NOTE: Here we have to use the base class get! Otherwise we only would get SETs
        Hypergraph *base = sys;
        Relation *rel = Relation::promote(base->get(relId)); // TODO: Make this a sys->method?
        // Get all sets containing this relation AND having a certain label
        auto edges = rel->pointedBy(sys, label);
        for (auto edgeId : edges)
        {
            // Insert if it is a set!
            if (sys->isSet(edgeId))
                result.insert(edgeId);
        }
    }

    return result;
}

bool Set::memberOf(SetSystem *sys, const unsigned otherId)
{
    return sys->relateTo(id(), otherId, Relation::memberOfLabel); // i am member of other
}
