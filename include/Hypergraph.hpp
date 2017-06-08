#ifndef _HYPERGRAPH_HPP
#define _HYPERGRAPH_HPP

#include <map>
#include <set>

// Forward declarations
class Hyperedge;

/*
    The hypergraph class serves as a factory and repository for hyperedges
    Within this environment IDs are unique and labels don't have to be unique

    A hypergraph stores hyperedges which are placeholders for id -> {id} and id -> string.
    The hypergraph therefore consists essentially of two tables (see HypergraphDB):
    id | id1 ... idN    and    id | string
    ---|------------           ---|-------
*/

class Hypergraph {
    public:
        typedef std::set<unsigned> Hyperedges;

        Hypergraph();
        Hypergraph(Hypergraph& A, Hypergraph& B);           // creates a new hypergraph out of two given ones
        ~Hypergraph();

        /*Factory functions for member edges*/
        unsigned create(const std::string& label="");                   // creates a new hyperedge
        unsigned create(Hyperedges fromEdges, Hyperedges toEdges, const std::string& label=""); // created a new hyperedge pointing to others
        bool     create(const unsigned id, 
                        const std::string& label="");                   // Tries to create a hyperedge with a given id ... if already taken, returns false
        void destroy(const unsigned id);                                // Will remove a hyperedge from this hypergraph (and also disconnect it from anybody)

        /*Get access to edges*/
        Hyperedge* get(const unsigned id);                              // Provides access to the hyperedge given by id
        Hyperedges find(const std::string& label="") const;             // Finds all hyperedges with a certain label

        /*Connect edges*/
        bool to(const unsigned srcId, const unsigned destId);           // Afterwards srcId will point to destId. But the converse is not true!!!
        bool to(const unsigned srcId, const Hyperedges others);         // Convenience function
        bool from(const unsigned srcId, const unsigned destId);         // Afterwards destId will point from srcId. But the converse is not true!!!
        bool from(const Hyperedges others, const unsigned destId);      // Convenience function
        void disconnect(const unsigned id);                             // Disconnects edge from all other edges (this means finding all edges which reference the given id)

        /*Traverse connected subgraphs*/
        enum TraversalDirection {
            DOWN,   // in direction of the _to set
            UP,     // in direction of the _from set
            BOTH    // in direction of both
        };

        /*Traversal which returns all visited edges*/
        template <typename ResultFilter, typename TraversalFilter> Hyperedges traversal
        ( 
            const unsigned rootId,              // The starting edge
            ResultFilter f,
            TraversalFilter g,
            const TraversalDirection dir = DOWN
        );
        /*Traversal which returns an hyperedge pointing to all results*/
        template <typename ResultFilter, typename TraversalFilter> unsigned traversal(
            const unsigned rootId,                  // The starting edge
            ResultFilter f,                         // Unary function bool f(Hyperedge *)
            TraversalFilter g,                      // Binary function bool g(Hyperedge *next, Hyperedge *current)
            const std::string& label,               // Label for the result hyperedge
            const TraversalDirection dir = DOWN     // Direction of traversal
        );

        /* Merge operations on hyperedges*/
        unsigned unite(const unsigned idA, const unsigned idB);        // Unites the to/from sets of A and B creating a new edge C
        unsigned intersect(const unsigned idA, const unsigned idB);    // Intersect the to/from sets of A and B creating a new edge C
        unsigned subtract(const unsigned idA, const unsigned idB);     // Create an edge C which contains all edges A points to/from but B does not point to/from

    protected:

        /*Id generation: Its like a ticketing system, once you called this, you have to use it!*/
        unsigned getNextId();

        // Private members for factory
        unsigned _lastId;                      // this is the id we can safely assign but should increase whenever we used it
        std::map<unsigned, Hyperedge> _edges;  // stores all hyperedges in a map id -> hyperedge
};

// Include template member functions
// See http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
#include "Hypergraph.tpp"

#endif
