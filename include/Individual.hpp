#ifndef _INDIVIDUAL_HPP
#define _INDIVIDUAL_HPP

#include "Hyperedge.hpp"

// A proper individual is a 0-Hyperedge
class Individual : public Hyperedge
{
    public:
        Individual(const std::string& label="")
        : Hyperedge(label)
        {
        }
};

#endif

