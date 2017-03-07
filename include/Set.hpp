#ifndef _SET_HPP
#define _SET_HPP

#include "Hyperedge.hpp"

// A set is just a hyperedge
class Set : public Hyperedge
{
    public:
        Set(const std::string& label="");
        Set(Hyperedge::Hyperedges members, const std::string& label);
};

#endif
