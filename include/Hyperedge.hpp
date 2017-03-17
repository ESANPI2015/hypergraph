#ifndef _HYPEREDGE_HPP
#define _HYPEREDGE_HPP

#include <map>
#include <string>

/*
* This is the main primitive of a generalized hypergraph: the hyperedge
* This hyperedge can point to other hyperedges.
* Note, that nodes are not needed anymore since they are special, 0-hyperedges.
* You can however derive them from 0-Hyperedges.
*
* TODO:
* Need some form of UUID: Current idea is to use label and date/time for hashing
* -> UNIQUE LABELS!!! LABELS ARE NOT STORAGE! -> But it should be somehting like a name!?!
* Should we make this a template class? So then we could store some data in it
* Serialization is just a special case of graph traversal. Make this more generic
* Actually we should make both, serializer & deserializer static or not. In the latter case we can make them virtual
*  in the former case we decouple them from the Hyperedge core functionality. However deserialize should either be static OR
*  a constructor but not a modifier. But we cannot have virtual constructors
* -> Both static
*/
class Hyperedge
{
    public:
        typedef std::map<unsigned, Hyperedge*> Hyperedges;

        /*Constructors*/
        Hyperedge(const std::string& label="");
        Hyperedge(Hyperedges members, const std::string& label="");
        
        /*Destructor*/
        ~Hyperedge();

        /*Factory functions*/
        static Hyperedge* create(const std::string& label="");
        static Hyperedge* create(Hyperedges members, const std::string& label="");
        static void cleanup();

        /*Special factory functions for import/export*/
        static std::string serialize(Hyperedge* root); // DFS
        static Hyperedge* deserialize(const std::string& from);

        /*
            Read Access:
            Note, the members() and supers() functions can filter by label (labelContains behavior)
        */
        unsigned id() const; 
        std::string label() const; 
        unsigned cardinality() const;
        Hyperedges members(const std::string& label="");
        Hyperedges supers(const std::string& label="");

        /*
            Write access:
            This is convenient to use (but the constructor above is the only thing necessary)
        */
        bool contains(Hyperedge *member);
        void clear(); // Removes all members (and also deregisters)
        void seperate(); // Removes from all super sets (and also deregisters)
        void detach(); // Combination of clear and seperate

        /*TODO: To ensure uniqueness and other things we have to override == and other operators*/
        
        /*
            Graph traversals/Queries
            NOTE: These functions create new hyperedges
        */
        enum TraversalDirection {
            DOWN,
            UP,
            BOTH
        };
        template <typename Func> Hyperedge* traversal(Func f, const std::string& label="Traversal", const TraversalDirection dir = DOWN);
        Hyperedge* labelContains(const std::string& str="", const TraversalDirection dir = DOWN);
        Hyperedge* labelPartOf(const std::string& str="", const TraversalDirection dir = DOWN);
        Hyperedge* cardinalityLessThanOrEqual(const unsigned cardinality=0, const TraversalDirection dir = DOWN);
        Hyperedge* cardinalityGreaterThan(const unsigned cardinality=0, const TraversalDirection dir = DOWN);

        /*
            Merge operations
            NOTE: The returned Hyperedges are NEW Hyperedges
        */
        // Unite *this and other (but *this and other are NOT part of unification)
        Hyperedge* unite(const Hyperedge* other);
        // Intersect *this and other (and *this and other cannot be part of it)
        Hyperedge* intersect(const Hyperedge* other);
        // Difference between *this - other (and *this and other cannot be part of it)
        Hyperedge* subtract(const Hyperedge* other);
        // Difference between other - *this (and *this and other cannot be part of it)
        Hyperedge* complement(Hyperedge* other);

    private:
        // Private traversal function which will NOT create any edges (to be used by functions called on creation like contains)
        template <typename Func> Hyperedges _traversal(Func f, const TraversalDirection dir = DOWN);

        // Private factory functions to create Hyperedges with a certain id
        // Used by deserialization
        static Hyperedge* create(const unsigned id, const std::string& label="");
        static Hyperedge* create(const unsigned id, Hyperedges members, const std::string& label="");

        // Private members
        unsigned _id;
        std::string _label;
        Hyperedges _supers;  // This is the row of an incidence matrix
        Hyperedges _members; // This is the column of an incidence matrix

        // Private static members for 
        static unsigned _lastId;
        static Hyperedges _created;
};
#endif
