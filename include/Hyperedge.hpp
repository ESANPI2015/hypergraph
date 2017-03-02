#ifndef _HYPEREDGE_HPP
#define _HYPEREDGE_HPP

#include <vector>
#include <string>

/*
* This is the main primitive of a generalized hypergraph: the hyperedge
* This hyperedge can point to other hyperedges.
* Note, that nodes are not needed anymore since they are special, 0-hyperedges.
* You can however derive them from 0-Hyperedges.
*/
class Hyperedge
{
    public:
        typedef std::vector<Hyperedge*> Hyperedges;

        /*Constructors*/
        Hyperedge(const std::string& label="");
        Hyperedge(Hyperedges members, const std::string& label="");

        /*Read Access*/
        std::string label() const; 
        Hyperedges members(const std::string& label="");

    private:
        std::string _label;
        Hyperedges _members;
};

#endif
