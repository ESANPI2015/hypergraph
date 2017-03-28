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

        // Write access
        bool memberOf(Set *other); // causes a memberOf relation to be created
        bool isA(Set *other);      // causes a isA relation to be created 
        bool partOf(Set *other);   // causes a partOf relation to be created

        // Useful static member functions
        static Set* promote(Hyperedge *edge);
        static Sets promote(Hyperedge::Hyperedges edges);

        // Queries:
        // Transitive closures of ...
        Set* memberOf();
        Set* kindOf(); //isA
        Set* partOf();
};

#endif
