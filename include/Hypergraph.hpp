#ifndef _HYPERGRAPH_HPP
#define _HYPERGRAPH_HPP

#include <vector>
#include <map>
#include <set>
#include <stack>
#include "Hyperedge.hpp"

/*
    The hypergraph class serves as a factory and repository for hyperedges
    Within this environment IDs are unique and labels don't have to be unique

    A hypergraph stores hyperedges which are placeholders for id -> {id} and id -> string.
    The hypergraph therefore consists essentially of two tables (see HypergraphDB):
    id | id1 ... idN    and    id | string
    ---|------------           ---|-------

    We want to enforce unique ids, so ids will not be assigned automatically but it can be checked if it is available
*/

typedef std::map<UniqueId, UniqueId> Mapping;   //< This map stores a one-to-one mapping between hedges (IDs)
bool equal(const Mapping& a, const Mapping& b); //< Check if two mappings are equal or not
Mapping invert(const Mapping& m);               //< Returns the inverse mapping
Mapping fromHyperedges(const Hyperedges& a);    //< Constructs a identity mapping between the elements of a
Mapping join(const Mapping& a, const Mapping& b); //< Constructs from two mappings the inner join: a:X->Y, b:X->Z --> result: Y->Z
std::ostream& operator<< (std::ostream& os , const Mapping& val);

class Hypergraph {
    public:
        Hypergraph();
        Hypergraph(const Hypergraph& other);                // copy constructor to repopulate the hyperedge cache(s)
        Hypergraph(const Hypergraph& A, const Hypergraph& B);           // creates a new hypergraph out of two given ones
        ~Hypergraph();

        /*Factory functions for member edges*/
        Hyperedges create(const UniqueId id, 
                          const std::string& label="");                 // Tries to create a hyperedge with a given id ... if already taken, returns empty set
        void destroy(const UniqueId id);                                // Will remove a hyperedge from this hypergraph (and also disconnect it from anybody)

        /*Get access to edges*/
        Hyperedge* get(const UniqueId id);                              // Provides access to the hyperedge given by id
        Hyperedges find(const std::string& label="") const;             // Finds all hyperedges with a certain label

        /*Connect edges*/
        Hyperedges to(const Hyperedges& srcIds, const Hyperedges& others);    // Afterwards every srcId in srcIds will point to others. The converse is not true!!
        Hyperedges from(const Hyperedges& others, const Hyperedges& destIds); // Afterwards every destId in destIds will point from others. The converse is not true!!
        void disconnect(const UniqueId id);                                  // Disconnects edge from all other edges (this means finding all edges which reference the given id)

        /*Queries*/
        unsigned size() { return _edges.size(); };
        Hyperedges from(const Hyperedges& ids, const std::string& label=""); // Returns all hyperedges from which each id in ids points, filtered by label
        Hyperedges to(const Hyperedges& ids, const std::string& label="");   // Returns all hyperedges to which each id in ids points, filtered by label

        /*Adjacency queries*/
        Hyperedges prevNeighboursOf(const Hyperedges& ids, const std::string& label=""); // Returns all hyperedges which a) are in id's from set or b) have id in their to set
        Hyperedges nextNeighboursOf(const Hyperedges& ids, const std::string& label=""); // Returns all hyperedges which a) are in id's to set or b) have id in their from set
        Hyperedges allNeighboursOf(const Hyperedges& ids, const std::string& label="");  // Returns all hyperedges which a) are in id's to||from sets or b) have id in their to||from set

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
            const UniqueId rootId,                  // The starting edge
            ResultFilter f,                         // Unary function bool f(Hyperedge *)
            TraversalFilter g,                      // Binary function bool g(Hyperedge *current, Hyperedge *next)
            const TraversalDirection dir = DOWN
        );

        /* Pattern matching */
        Mapping match(Hypergraph& other,                                                       //< Find embedding of other graph in this graph
                      std::stack< Mapping >& searchSpace                                       //< Uses this to use/store the state of the search
                     );

        /* Graph rewriting: single pushout */
        // NOTE: Partial Map means, that hedges in lhs do not need to be mapped to hedges in rhs
        Hypergraph rewrite(Hypergraph& lhs,                     //< The left hand side graph to be matched to the data graph
                           Hypergraph& rhs,                     //< The right hand side to replace the matched subgraph with
                           const Mapping& partialMap,           //< A partial map from the lhs to the rhs (TODO: currently N:1, should become N:M)
                           std::stack< Mapping >& searchSpace   //< The search space of the matching phase for reusage
                          );

    protected:
        // Private members for factory
        std::map<UniqueId, Hyperedge> _edges;  // stores all hyperedges in a map id -> hyperedge
};

// Include template member functions
// See http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
#include "Hypergraph.tpp"

#endif
