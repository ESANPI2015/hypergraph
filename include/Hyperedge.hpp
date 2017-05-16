#ifndef _HYPEREDGE_HPP
#define _HYPEREDGE_HPP

#include <map>
#include <set>
#include <string>

/*
* This is the main primitive of a generalized hypergraph: the hyperedge
* This hyperedge can point to other hyperedges.
* Note, that nodes are not needed anymore since they are special, 0-hyperedges.
* You can however derive them from 0-Hyperedges.
* NOTE: Only ids are stored ... pointers are avoided as much as possible
*/

class Hyperedge
{
    public:
        typedef std::set<unsigned> Hyperedges;

        /*Destructor*/
        ~Hyperedge();

        /*Factory functions*/
        static Hyperedge* create(const std::string& label="");
        static Hyperedge* create(Hyperedges edges, const std::string& label="");
        // Finds a hyperedge by id
        static Hyperedge* find(const unsigned id);
        // Destroys all hyperedges
        static void cleanup();
        // Tries to create a hyperedge with a given id ... if already taken, returns NULL
        static Hyperedge* create(const unsigned id, const std::string& label="");

        /*
            Read Access
        */
        unsigned id() const; 
        std::string label() const; 
        unsigned cardinality() const;
        Hyperedges pointingTo(const std::string& label="") const;
        Hyperedges pointedBy(const std::string& label="") const;
        Hyperedge *pointingTo(const unsigned id);
        Hyperedge *pointedBy(const unsigned id);

        /*
            Write access
        */
        void updateLabel(const std::string& label);
        bool pointTo(const unsigned id); // Adds the edge to the set of edges we point to (and also registers in their from set)
        bool pointTo(Hyperedge *other);

        /*
            Removal
        */
        void clear(); // Removes all hyperedges we point to (and also deregisters)
        void seperate(); // Removes all hyperedges pointing to us (and also deregisters)
        void detach(); // Combination of clear and seperate

        /*
            Graph traversals/Queries producing new hyperedges
        */
        enum TraversalDirection {
            DOWN,   // in direction of the _to set
            UP,     // in direction of the _from set
            BOTH    // in direction of both
        };
        template <typename ResultFilter, typename TraversalFilter> Hyperedge* traversal(
            ResultFilter f,                         // Unary function bool f(Hyperedge *)
            TraversalFilter g,                      // Binary function bool g(Hyperedge *next, Hyperedge *current)
            const std::string& label="Traversal",   // Label for the result hyperedge
            const TraversalDirection dir = DOWN     // Direction of traversal
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
        friend std::ostream& operator<< (std::ostream& stream, const Hyperedge* edge);
        static std::string serialize(Hyperedge* root); // NOTE: Only serializes connected systems!!!

    private:
        /*Constructors: Private to allow only heap objects*/
        Hyperedge(const std::string& label="");
        Hyperedge(const Hyperedge&);
        Hyperedge& operator=(const Hyperedge&);
        
        // Private traversal function which will NOT create any edges (to be used by functions called on creation like contains)
        template <typename ResultFilter, typename TraversalFilter> Hyperedges _traversal
        ( 
            ResultFilter f,
            TraversalFilter g,
            const TraversalDirection dir = DOWN
        );

        // Private members
        unsigned _id;
        std::string _label;
        Hyperedges  _from;  // This is the row of an incidence matrix
        Hyperedges  _to;    // This is the column of an incidence matrix

        // Private static members for factory
        static unsigned _lastId;                      // use this if no id has been provided and update it
        static std::map<unsigned, Hyperedge*> _edges; // stores all dynamically created hyperedges
};

// Include template member functions
// See http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
#include "Hyperedge.tpp"

#endif
