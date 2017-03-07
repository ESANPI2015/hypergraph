#ifndef _RELATION_HPP
#define _RELATION_HPP

#include "Set.hpp"

// Are relations just simple sets? Yes :)
class Relation : public Set
{
    public:
        Relation(const std::string& label="");
        Relation(Hyperedge::Hyperedges members, const std::string& label="");
};

#endif
