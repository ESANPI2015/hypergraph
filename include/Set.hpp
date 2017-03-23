#ifndef _SET_HPP
#define _SET_HPP

#include "Relation.hpp"

// A set (system) is formed by Hyperedges and a memberOf relation
class Set : public Hyperedge
{
    public:
        typedef std::map<unsigned, Set*> Sets;

        Set(const std::string& label="");
        Set(Sets members, const std::string& label);

        // Factory function
        static Set* create(const std::string& label="");

        // Write access
        // NOTE: also causes a memberOf relation to be created
        bool contains(Set *other);
};

#endif
