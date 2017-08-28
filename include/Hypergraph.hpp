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

    We want to enforce unique ids, so ids will not be assigned automatically but it can be checked if it is available
*/

class Hypergraph {
    public:
        typedef std::set<unsigned> Hyperedges;

        Hypergraph();
        Hypergraph(Hypergraph& A, Hypergraph& B);           // creates a new hypergraph out of two given ones
        ~Hypergraph();

        /*Factory functions for member edges*/
        Hyperedges create(const unsigned id, 
                          const std::string& label="");                 // Tries to create a hyperedge with a given id ... if already taken, returns empty set
        void destroy(const unsigned id);                                // Will remove a hyperedge from this hypergraph (and also disconnect it from anybody)

        /*Get access to edges*/
        Hyperedge* get(const unsigned id);                              // Provides access to the hyperedge given by id
        Hyperedges find(const std::string& label="");                   // Finds all hyperedges with a certain label

        /*Connect edges*/
        Hyperedges to(const unsigned srcId, const unsigned destId);       // Afterwards srcId will point to destId. But the converse is not true!!!
        Hyperedges to(const unsigned srcId, const Hyperedges& others);    // Convenience function
        Hyperedges from(const unsigned srcId, const unsigned destId);     // Afterwards destId will point from srcId. But the converse is not true!!!
        Hyperedges from(const Hyperedges& others, const unsigned destId); // Convenience function
        void disconnect(const unsigned id);                               // Disconnects edge from all other edges (this means finding all edges which reference the given id)

        /* */
        Hyperedges from(const unsigned id, const std::string& label="");     // Returns all hyperedges from which id points (filtered by label)
        Hyperedges to(const unsigned id, const std::string& label="");       // Returns all hyperedges to which id points (filtered by label)
        Hyperedges from(const Hyperedges& ids, const std::string& label=""); // Nice for chaining
        Hyperedges to(const Hyperedges& ids, const std::string& label="");   // Nice for chaining

        /*Adjacency queries*/
        Hyperedges prevNeighboursOf(const unsigned id, const std::string& label=""); // Returns all hyperedges which a) are in id's from set or b) have id in their to set
        Hyperedges nextNeighboursOf(const unsigned id, const std::string& label=""); // Returns all hyperedges which a) are in id's to set or b) have id in their from set
        Hyperedges allNeighboursOf(const unsigned id, const std::string& label="");  // Returns all hyperedges which a) are in id's to||from sets or b) have id in their to||from set
        Hyperedges prevNeighboursOf(const Hyperedges& ids, const std::string& label=""); // Nice for chaining 
        Hyperedges nextNeighboursOf(const Hyperedges& ids, const std::string& label=""); // Nice for chaining 
        Hyperedges allNeighboursOf(const Hyperedges& ids, const std::string& label="");  // Nice for chaining

        /*Traverse connected subgraphs*/
        enum TraversalDirection {
            DOWN,   // in direction of the hedges
            UP,     // against the direction of the hedges
            BOTH    // in both directions
        };
        /*Traversal which returns all visited edges*/
        // TODO: Rethink signature of functors f and g
        template <typename ResultFilter, typename TraversalFilter> Hyperedges traversal
        ( 
            const unsigned rootId,                  // The starting edge
            ResultFilter f,                         // Unary function bool f(Hyperedge *)
            TraversalFilter g,                      // Binary function bool g(Hyperedge *current, Hyperedge *next)
            const TraversalDirection dir = DOWN
        );

        /* Merge operations on hyperedge sets */
        Hyperedges unite(const Hyperedges& edgesA, const Hyperedges& edgesB);       // Unites the two hyperedge sets to a new one
        Hyperedges intersect(const Hyperedges& edgesA, const Hyperedges& edgesB);   // Intersects the two hyperedge sets and returns the result (DLOG)
        Hyperedges subtract(const Hyperedges& edgesA, const Hyperedges& edgesB);    // Returns all edges which are in A but not in B
        unsigned first(const Hyperedges& edges);                                    //< Return the first of the edge set

        /* Pattern matching */
        typedef std::map<unsigned, unsigned> Mapping;                               //< This map stores a one-to-one mapping between hedges (IDs)
        Mapping match(const Hyperedges& otherIds);                                  //< In-place matching (Subgraph embedded in the same graph)
        Mapping rewrite(Mapping& matched, Mapping& replacements);                   //< In-place rewrite: Given a match and a replacement, the graph will be transformed

    protected:
        // Private members for factory
        std::map<unsigned, Hyperedge> _edges;  // stores all hyperedges in a map id -> hyperedge
};

// Include template member functions
// See http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
#include "Hypergraph.tpp"

#endif
