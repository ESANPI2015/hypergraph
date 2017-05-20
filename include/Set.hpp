#ifndef _SET_HPP
#define _SET_HPP

#include "Relation.hpp"

// A set (system) is formed by Hyperedges and a memberOf relation
// A subsumption hierarchy is formed by an isA relation
// A compositional hierarchy is formed by an partOf relation
class Set : public Hyperedge
{
    public:
        typedef std::set<unsigned> Sets;
        static const std::string   superclassLabel;                          // The superclassLabel is a string representing the class of sets

        /* Constructor */
        Set(const unsigned id, const std::string& label="");

        /* Read access */
        Sets members(SetSystem *sys, const std::string& label="");     // searches for memberOf relations from other sets with a certain label
        //Sets kindOf(SetSystem *sys, const std::string& label="");      // searches for isA relations to other Sets with a certain label
        //Sets partOf(SetSystem *sys, const std::string& label="");      // searches for partOf relations to other Sets with a certain label

        /* Write access */
        bool memberOf(SetSystem *sys, const unsigned otherId);               // causes a memberOf relation to be created
        //bool isA(SetSystem* sys, const unsigned superclassId);               // causes a isA relation to be created 
        //bool partOf(SetSystem* sys, const unsigned containerId);             // causes a partOf relation to be created
};

#endif
