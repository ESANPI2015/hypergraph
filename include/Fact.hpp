#ifndef _FACT_HPP
#define _FACT_HPP

#include "Set.hpp"
#include "Individual.hpp"

// A proper fact is a Set with at least two members which are only 0-hyperedges/Individuals
class Fact : public Set
{
    public:
        /*Constructors*/
        Fact(Hyperedges members, const std::string& label="");
};

#endif

