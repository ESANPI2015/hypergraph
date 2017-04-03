#ifndef _SET_HPP
#define _SET_HPP

#include "Relation.hpp"

// A set (system) is formed by Hyperedges and a memberOf relation
// A subsumption hierarchy is formed by an isA relation
// A compositional hierarchy is formed by an partOf relation
class Set : public Hyperedge
{
    public:
        typedef std::map<unsigned, Set*> Sets;

        Set(const std::string& label="");
        Set(Sets members, const std::string& label);

        // Factory function
        static Set* create(const std::string& label="");
        static Set* create(Sets members, const std::string& label="");

        // Write access
        bool memberOf(Set *other); // causes a memberOf relation to be created
        bool isA(Set *other);      // causes a isA relation to be created 
        bool partOf(Set *other);   // causes a partOf relation to be created

        // Useful static member functions
        static Set* promote(Hyperedge *edge);
        static Sets promote(Hyperedge::Hyperedges edges);

        // Read access
        Sets directMembers(const std::string& label="") const; // Only the DIRECT members of the set (for transitive sets see members() below)

        // Queries:
        // Transitive closures of ...
        Relation* memberOf();
        Relation* kindOf(); //isA
        Relation* partOf();

        // Inverse transitive closures (temporary relations only)
        Set* members(); // All members AND their members AND ...
        Set* subclasses();
        Set* parts();

        // Merge operations:
        // NOTE: These operations are USING the memberOf relation ONLY!!!
        //       They also use only direct members
        // Unite *this and other (but *this and other are NOT part of unification)
        Set* unite(const Set* other);
        // Intersect *this and other (and *this and other cannot be part of it)
        Set* intersect(const Set* other);
        // Difference between *this - other (and *this and other cannot be part of it)
        Set* subtract(const Set* other);
};

#endif
