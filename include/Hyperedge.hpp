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
        Hyperedges pointingTo(const std::string& label="") const;
        Hyperedges pointedBy(const std::string& label="") const;

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
            DOWN,   // in direction of the _to set
            UP,     // in direction of the _from set
            BOTH    // in direction of both
        };
        template <typename T, typename ResultFilter, typename TraversalFilter> T* traversal(
            ResultFilter f,                         // Unary function bool f(Hyperedge *)
            TraversalFilter g,                      // Binary function bool g(Hyperedge *next, Hyperedge *current)
            const std::string& label="Traversal",   // Label for the result hyperedge
            const TraversalDirection dir = DOWN     // Direction of traversal
        );

        template <typename T> T* labelContains(const std::string& str="");
        template <typename T> T* labelPartOf(const std::string& str="");
        template <typename T> T* cardinalityLessThanOrEqual(const unsigned cardinality=0);
        template <typename T> T* cardinalityGreaterThan(const unsigned cardinality=0);
        template <typename T> T* successors(); // Go in direction of to set and register all edges AND edge of edges
        template <typename T> T* predecessors(); // Go in direction of from set and register all edges AND edges of edges

        /*
            Merge operations producing new hyperedges
            Templated to allow it to work for derived classes as well
        */
        // Unite *this and other (but *this and other are NOT part of unification)
        template <typename T> T* unite(const T* other);
        // Intersect *this and other (and *this and other cannot be part of it)
        template <typename T> T* intersect(const T* other);
        // Difference between *this - other (and *this and other cannot be part of it)
        template <typename T> T* subtract(const T* other);

        /* Serialization */
        friend std::ostream& operator<< (std::ostream& stream, const Hyperedge* edge);
        static std::string serialize(Hyperedge* root);

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

// Include template member functions
// See http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
#include "Hyperedge.tpp"

#endif
