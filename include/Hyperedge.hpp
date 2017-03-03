#ifndef _HYPEREDGE_HPP
#define _HYPEREDGE_HPP

#include <vector>
#include <string>

/*
* This is the main primitive of a generalized hypergraph: the hyperedge
* This hyperedge can point to other hyperedges.
* Note, that nodes are not needed anymore since they are special, 0-hyperedges.
* You can however derive them from 0-Hyperedges.
*
* TODO:
* Need some form of UUID: Current idea is to use label and date/time for hashing -> UNIQUE LABELS!!! LABELS ARE NOT STORAGE!
* Should we make this a template class? So then we could store some data in it
* Serialization is just a special case of graph traversal. Make this more generic
* Actually we should make both, serializer & deserializer static or not. In the latter case we can make them virtual
*  in the former case we decouple them from the Hyperedge core functionality. However deserialize should either be static OR
*  a constructor but not a modifier. But we cannot have virtual constructors -> Both static
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

        /*Graph traversal*/
        static std::string serialize(Hyperedge* root); // DFS
        static Hyperedge* deserialize(const std::string& from);

    private:
        std::string _label;
        Hyperedges _members;
};
#endif
