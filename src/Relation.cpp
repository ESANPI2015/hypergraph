#include "Relation.hpp"
#include "Set.hpp"
#include "SetSystem.hpp"

const std::string Relation::isALabel = "isA";
const std::string Relation::memberOfLabel = "memberOf";
const std::string Relation::partOfLabel = "partOf";
const std::string Relation::hasLabel = "hasA";
const std::string Relation::connectedToLabel = "connectedTo";

const std::string Relation::inverseIsALabel = "superclassOf";
const std::string Relation::inverseMemberOfLabel = "setOf";
const std::string Relation::inversePartOfLabel = "wholeOf";
const std::string Relation::inverseHasLabel = "ownerOf";
const std::string Relation::inverseConnectedToLabel = "connectedTo";

Relation::Relation(const unsigned id, const std::string& label)
: Hyperedge(id,label)
{
}

Relation* Relation::promote(Hyperedge *edge)
{
    Relation* neu = static_cast<Relation*>(edge);
    return neu;
}

bool Relation::from(SetSystem *system, const unsigned id)
{
    return system->get(id)->pointTo(system, this->id());
}

bool Relation::to(SetSystem *system, const unsigned id)
{
    return pointTo(system, id);
}

bool Relation::fromTo(SetSystem* system, const unsigned fromId, const unsigned toId)
{
    return (from(system,fromId) && to(system, toId));
}
