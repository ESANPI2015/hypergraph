#ifndef _HYPERGRAPH_HPP
#define _HYPERGRAPH_HPP

#include <vector>
#include <map>
#include <unordered_map>
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

    NOTES:
    * get() returns a pointer instead of a reference to provide NULL if Hyperedge does not exist.
      The alternative is to define a special NULL Hyperedge (with id "0" or "NULL"). This will however make all 'if(get(id))' snippets invalid.
      (See nice explanations here: https://stackoverflow.com/questions/10371094/returning-a-null-reference-in-c)
    * An additional way to optimize scalability of this approach is to use hashmaps instead of normal maps.
      Then lookup can be done in O(1) (average). However, this could be premature optimization.
*/

typedef std::multimap<UniqueId, UniqueId> Mapping;   //< This map stores a many-to-many mapping between hedges (IDs)
bool equal(const Mapping& a, const Mapping& b);      //< Check if two mappings are equal or not
Mapping invert(const Mapping& m);                    //< Returns the inverse mapping
Mapping fromHyperedges(const Hyperedges& a);         //< Constructs a identity mapping between the elements of a
Mapping join(const Mapping& a, const Mapping& b);    //< Constructs from two mappings the inner join: a:X->Y, b:X->Z --> result: Y->Z
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
        void importFrom(const Hypergraph& other);                 // Imports all hyperedges from other graph (unless they already exist)

        /*Get access to edges*/
        bool exists(const UniqueId& uid) const;                         // Check if a hedge with uid exists
        const Hyperedge& read(const UniqueId id) const;                 // Give read-only access to a hyperedge
        Hyperedge* get(const UniqueId id);                              // Provides access to the hyperedge given by id.
        Hyperedges find(const std::string& label="") const;             // Finds all hyperedges with a certain label

        /*Connect edges*/
        Hyperedges to(const Hyperedges& srcIds, const Hyperedges& others);    // Afterwards every srcId in srcIds will point to others. The converse is not true!!
        Hyperedges from(const Hyperedges& others, const Hyperedges& destIds); // Afterwards every destId in destIds will point from others. The converse is not true!!
        void disconnect(const UniqueId id);                                  // Disconnects edge from all other edges (this means finding all edges which reference the given id)

        /*Queries*/
        unsigned size() const { return _edges.size(); }
        Hyperedges from(const Hyperedges& ids, const std::string& label="") const; // Returns all hyperedges from which each id in ids points, filtered by label
        Hyperedges to(const Hyperedges& ids, const std::string& label="") const;   // Returns all hyperedges to which each id in ids points, filtered by label

        /*Adjacency queries*/
        Hyperedges prevNeighboursOf(const Hyperedges& ids, const std::string& label="") const; // Returns all hyperedges which a) are in id's from set or b) have id in their to set
        Hyperedges nextNeighboursOf(const Hyperedges& ids, const std::string& label="") const; // Returns all hyperedges which a) are in id's to set or b) have id in their from set
        Hyperedges allNeighboursOf(const Hyperedges& ids, const std::string& label="") const;  // Returns all hyperedges which a) are in id's to||from sets or b) have id in their to||from set

        /*Traverse connected subgraphs*/
        enum TraversalDirection {
            FORWARD,    // in direction of the hedges
            INVERSE,    // against the direction of the hedges
            BOTH        // in both directions
        };
        /*Traversal which returns all visited edges*/
        template <typename ResultFilter, typename TraversalFilter> Hyperedges traversal
        ( 
            const UniqueId& rootId,                  // The starting edge
            ResultFilter f,                         // Unary function bool f(const Hyperedge&)
            TraversalFilter g,                      // Binary function bool g(const Hyperedge& current, const Hyperedge& next)
            const TraversalDirection dir = FORWARD
        ) const;

        /* Default matching function */
        static Hyperedges defaultMatchFunc(const Hypergraph& datagraph, const Hyperedge& queryHedge)
        {
            Hyperedges candidates;
            // Check uid
            if (datagraph.exists(queryHedge.id()))
            {
                candidates.push_back(queryHedge.id());
            } else {
                // ... in case the uid is not found, find by label
                candidates = datagraph.find(queryHedge.label());
            }
            // Filter by degree
            // Check in and out degrees here as well!! If candidate has LESS in or out degree it can not be a candidate
            Hyperedges filtered;
            for (const UniqueId& candidateId : candidates)
            {
                if (datagraph.read(candidateId).indegree() < queryHedge.indegree())
                    continue;
                if (datagraph.read(candidateId).outdegree() < queryHedge.outdegree())
                    continue;
                filtered.push_back(candidateId);
            }
            return filtered;
        }

        /* Pattern matching */
        template< typename MatchFunc > Mapping match(
                      const Hypergraph& other,                    //< The graph to be found in the current graph
                      std::stack< Mapping >& searchSpace,   //< A tree of the current state in search space.
                      MatchFunc m                           //< A binary function bool m(Hyperedge *, Hyperedge *) which decides if a hedge of other and a hedge of the current graph are candidates or not
                     ) const;

        /* Graph rewriting: single pushout */
        // NOTE: Partial Map means, that hedges in lhs do not need to be mapped to hedges in rhs (if not mapped, then they will get destroyed)
        template< typename MatchFunc > Hypergraph rewrite(
                            const Hypergraph& lhs,                     //< The matching graph
                            const Hypergraph& rhs,                     //< The replacment graph
                            const Mapping& partialMap,           //< A partial map from lhs to rhs (N:N)
                            std::stack< Mapping >& searchSpace,  //< The search space of the matching phase for reusage
                            MatchFunc mf                         //< A binary function bool m(Hyperedge *, Hyperedge *) which decides if a hedge of other and a hedge of the current graph are candidates or not
                          ) const;

    protected:
        // Stores all hyperedges belonging to a certain graph instance
        // For fast lookup, we use the UniqueId to retrieve the corresponding hyperedge
        std::unordered_map<UniqueId, Hyperedge> _edges;
};

// Include template member functions
// See http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
#include "Hypergraph.tpp"

#endif
