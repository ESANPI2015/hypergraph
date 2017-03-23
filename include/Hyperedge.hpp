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
* Should we make this a template class? So then we could store some data in it
*/
class Hyperedge
{
    public:
        typedef std::map<unsigned, Hyperedge*> Hyperedges;

        /*Constructors*/
        Hyperedge(const std::string& label="");
        Hyperedge(Hyperedges edges, const std::string& label="");
        
        /*Destructor*/
        ~Hyperedge();

        /*Factory functions*/
        static Hyperedge* create(const std::string& label="");
        static Hyperedge* create(Hyperedges edges, const std::string& label="");
        static void cleanup();

        /*
            Read Access
        */
        unsigned id() const; 
        std::string label() const; 
        unsigned cardinality() const;
        Hyperedges pointingTo(const std::string& label="");
        Hyperedges pointedBy(const std::string& label="");

        /*
            Write access
        */
        bool pointTo(Hyperedge* edge); // Adds the edge to the set of edges we point to (and also registers in their from set)
        void clear(); // Removes all hyperedges we point to (and also deregisters)
        void seperate(); // Removes all hyperedges pointing to us (and also deregisters)
        void detach(); // Combination of clear and seperate

        /*TODO: we have to override == and other operators to operate on _id*/
        
        /*
            Graph traversals/Queries producing new hyperedges
        */
        enum TraversalDirection {
            DOWN,
            UP,
            BOTH
        };
        template <typename ResultFilter, typename TraversalFilter> Hyperedge* traversal(
            ResultFilter f, 
            TraversalFilter g,
            const std::string& label="Traversal",
            const TraversalDirection dir = DOWN
        );
        Hyperedge* labelContains(const std::string& str="");
        Hyperedge* labelPartOf(const std::string& str="");
        Hyperedge* cardinalityLessThanOrEqual(const unsigned cardinality=0);
        Hyperedge* cardinalityGreaterThan(const unsigned cardinality=0);
        Hyperedge* successors(); // Go in direction of to set and register all edges AND edge of edges
        Hyperedge* predecessors(); // Go in direction of from set and register all edges AND edges of edges

        /*
            Merge operations producing new hyperedges
        */
        // Unite *this and other (but *this and other are NOT part of unification)
        Hyperedge* unite(const Hyperedge* other);
        // Intersect *this and other (and *this and other cannot be part of it)
        Hyperedge* intersect(const Hyperedge* other);
        // Difference between *this - other (and *this and other cannot be part of it)
        Hyperedge* subtract(const Hyperedge* other);

        /* Serialization */
        friend std::ostream& operator<< (std::ostream& stream, const Hyperedge& edge);

    protected:
        // Private traversal function which will NOT create any edges (to be used by functions called on creation like contains)
        template <typename ResultFilter, typename TraversalFilter> Hyperedges _traversal
        ( 
            ResultFilter f,
            TraversalFilter g,
            const TraversalDirection dir = DOWN
        );

        // Private factory functions to create Hyperedges with a certain id
        // Used by deserialization
        static Hyperedge* create(const unsigned id, const std::string& label="");
        static Hyperedge* create(const unsigned id, Hyperedges edges, const std::string& label="");

        // Private members
        unsigned _id;
        std::string _label;
        Hyperedges _from;  // This is the row of an incidence matrix
        Hyperedges _to;    // This is the column of an incidence matrix

        // Private static members for factory
        static unsigned _lastId;
        static Hyperedges _created;
};
#endif
