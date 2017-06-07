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
        ~Hypergraph();

        /*Factory functions for member edges*/
        unsigned create(const std::string& label="");               // creates a new hyperedge
        unsigned create(Hyperedges edges, const std::string& label="");
        bool     create(const unsigned id, 
                        const std::string& label="");               // Tries to create a hyperedge with a given id ... if already taken, returns false
        void destroy(const unsigned id);                            // Will remove a hyperedge from this hypergraph (and also disconnect it from anybody)

        /*Get access to edges*/
        Hyperedge* get(const unsigned id);                          // Finds a hyperedge by id
        // TODO: If we had a put method, we could get rid of pointers?
        Hyperedges find(const std::string& label="") const;         // Finds all hyperedges with a certain label

        /*Connect edges*/
        bool fromTo(const unsigned srcId, const unsigned destId);               // Connects two hyperedges
        void disconnect(const unsigned id);                                     // Disconnects edge from all other edges

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
        unsigned unite(const unsigned idA, const unsigned idB);        // Unites the to sets of A and B creating a new edge C
        unsigned intersect(const unsigned idA, const unsigned idB);    // Intersect the to sets of A and B creating a new edge C
        unsigned subtract(const unsigned idA, const unsigned idB);     // Create an edge C which contains all edges A points to but B does not point to

        /* TODO: We also need a possibility to check for hypergraph equality*/

        /* Merging hypergraphs */
        // This function constructs a new graph out of A and B which contains reindexed hedges of both graphs
        static Hypergraph* Union(Hypergraph* A, Hypergraph *B);

    protected:

        /*Id generation: Its like a ticketing system, once you called this, you have to use it!*/
        unsigned getNextId();

        // Private members for factory
        unsigned _lastId;                      // this is the id we can safely assign but should increase whenever we used it
        std::map<unsigned, Hyperedge*> _edges; // stores all dynamically created hyperedges (the ones this hypergraph created!) TODO: Do we need pointers? If not, we have to define equality and membership to a graph!
};

// Include template member functions
// See http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
#include "Hypergraph.tpp"

#endif
