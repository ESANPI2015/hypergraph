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

class Relation : public Hyperedge
{
    public:
        Relation(const std::string& label="");
        Relation(Hyperedge::Hyperedges from, Hyperedge::Hyperedges to, const std::string& label=""); // N-M relation constructor

        // Factory function
        static Relation* create(const std::string& label="");

        // Write access
        bool from(Hyperedge *source); // Adds an edge to the from set
        bool to(Hyperedge *target);   // Adds an edge to the to set
};

#endif
