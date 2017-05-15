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
*/

class Set;

class Relation : public Hyperedge
{
    public:
        // Promotion to Relation
        static Relation* promote(Hyperedge *edge);

        // Factory function
        static Relation* create(const std::string& label="relates");
        static Relation* create(Hyperedge::Hyperedges from, Hyperedge::Hyperedges to, const std::string& label="relates");

        // Write access
        bool from(const unsigned id); // Adds an edge to the from set
        bool from(Set *other); // Adds an edge to the from set
        bool to(const unsigned id);   // Adds an edge to the to set
        bool to(Set *other);   // Adds an edge to the to set

    private:
        // Private Constructors to prevent stack objects
        Relation(const std::string& label="relates");
        Relation(const Relation&);
        Relation& operator=(const Relation&);

};

#endif
