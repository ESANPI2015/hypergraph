#ifndef _SET_HPP
#define _SET_HPP

#include "Hyperedge.hpp"

// A proper set is a k-hyperedge with k >= 1
// TODO: We could make a template class out of set
class Set : public Hyperedge
{
    public:
        /*Constructors*/
        Set(Hyperedges members, const std::string& label="");
};

#endif
