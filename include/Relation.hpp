#ifndef _RELATION_HPP
#define _RELATION_HPP

#include "Hyperedge.hpp"

/* 
* A relation is something which connects different concepts with each other ... It 'relates' them.
* Example:
* PART X ---\
* ...         PART-OF ----> WHOLE Z
* PART Y ---/
*
* WHOLE is in the to set of the RELATION PART-OF
* PART X and PART Y are in the from set of it.
* 
* Encoding in a hypergraph
* Currently, the encoding of a relation is as follows:
* If r is a relation then there exists a hyperedge labelled Relation::Identifier which points to r.
* r is R <=> (HEDGE(R.Id) -> HEDGE(r))
* This way the relation itself as being a N:M HEDGE is not disturbed by the identifier.
* It is because HEDGE(R.Id) -> HEDGE(r) does not imply HEDGE(R.Id) <- HEDGE(r) !!!
*
*/

class Relation : public Hyperedge
{
    public:
        // Predefined label to encode a hyperedge as being interpretable as a relation
        static const std::string Identifier;

        // Constructor
        Relation(const unsigned id=0, const std::string& label="");

        // Checks if a relation is valid
        bool isValid();

        // TODO: Here you can place convenience functions ... but until there is nothing valuable, you don't have to.
};

#endif
