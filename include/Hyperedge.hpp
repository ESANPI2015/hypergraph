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
        Hyperedges supers(const std::string& label="");

        /*Write access*/
        bool contains(Hyperedge *member);

        /*Graph serialization: Order preserving!!! TODO: Could use traversal member func*/
        static std::string serialize(Hyperedge* root); // DFS
        static Hyperedge* deserialize(const std::string& from);
        
        /*TODO: To ensure uniqueness and other things we have to override == and other operators*/
        
        /*
            Graph traversals/Queries
            NOTE: These are collections of hyperedges but can be made permanent by creating a Hyperedge out of it
        */
        enum TraversalDirection {
            DOWN,
            UP,
            BOTH
        };
        template <typename Func> Hyperedges traversal(Func f, const TraversalDirection dir = DOWN);
        Hyperedges labelContains(const std::string& str="", const TraversalDirection dir = DOWN);
        Hyperedges labelPartOf(const std::string& str="", const TraversalDirection dir = DOWN);
        Hyperedges cardinalityLessThanOrEqual(const unsigned cardinality=0, const TraversalDirection dir = DOWN);
        Hyperedges cardinalityGreaterThan(const unsigned cardinality=0, const TraversalDirection dir = DOWN);

        * NOTE: If labels are equal than things are equal (by pointer as well!)
        */
        // Unite *this and other (but *this and other are NOT part of unification)
        Hyperedge unite(const Hyperedge& other, const std::string& name="Union");
        // Intersect *this and other (and *this and other cannot be part of it)
        Hyperedge intersect(const Hyperedge& other, const std::string& name="Intersection");
        // Difference between *this - other (and *this and other cannot be part of it)
        Hyperedge subtract(const Hyperedge& other, const std::string& name="Difference");
        // Difference between other - *this (and *this and other cannot be part of it)
        Hyperedge complement(const Hyperedge& other, const std::string& name="Difference");

    private:
        std::string _label;
        Hyperedges _supers;  // This is the row of an incidence matrix
        Hyperedges _members; // This is the column of an incidence matrix
};
#endif
