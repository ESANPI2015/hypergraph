#ifndef _RELATION_HPP
#define _RELATION_HPP

#include "Hyperedge.hpp"

/* 
* A relation is a hyperedge which points to all entities (hyperedges) in the target set
* Example:
* PART X ---\
* ...         PART-OF ----> WHOLE
* PART Y ---/
*
* WHOLE is in the to set of the RELATION PART-OF
* PART X and PART Y are in the from set of it.
*
* Relations are part of a set system which is based on a hypergraph
*/

class Set;
class SetSystem;

class Relation : public Hyperedge
{
    public:
        // Constructor
        Relation(const unsigned id, const std::string& label="");

        // Predefined labels for fundamental relations
        static const std::string isALabel;
        static const std::string memberOfLabel;
        static const std::string partOfLabel;
        static const std::string hasLabel;
        static const std::string connectedToLabel;

        // Predefined labels for the inverses of the relations
        static const std::string inverseIsALabel;
        static const std::string inverseMemberOfLabel;
        static const std::string inversePartOfLabel;
        static const std::string inverseHasLabel;
        static const std::string inverseConnectedToLabel;

        // Promotion to Relation
        // This is a shortcut casting mechanism
        static Relation* promote(Hyperedge *edge);

        /*Graph dependent operations*/
        bool from(SetSystem* system, const unsigned fromId);                        // fromId --> this
        bool to(SetSystem* system, const unsigned toId);                            // this --> toId
        bool fromTo(SetSystem* system, const unsigned fromId, const unsigned toId); // fromId --> this --> toId
};

#endif
