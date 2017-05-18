#ifndef _HYPERGRAPH_HPP
#define _HYPERGRAPH_HPP

#include <map>
#include <set>

// Forward declarations
class Hyperedge;

/*
    The hypergraph class serves as a factory and repository for hyperedges
    Within this environment IDs are unique and labels don't have to be unique

    NOTE: We inherit from Hyperedge to be able to easily build a multiverse of hypergraphs
    Consider the analogy between table of tables in a DATABASE
    
    Q: How do we link edge X which is a subedge of A to an edge Y being a subedge of B?
    A: Merge A with B creating a edge C ... now X and Y are both part of C and can be linked in the context of C :)
    TODO: Be careful when releasing the memory though ... who is responsible for that?

    FIXME: When creating a new hypergraph, the hyperedge _id gets initialized with the _lastId of a possible higher level graph or 0!
    So globally, when we have multiple graphs which are not nested, we then have many hyperedges with the same id 0!
    To avoid that, a MASTER HYPERGRAPH (call it Universe?) has to be created initially which refers to all SUB HYPERGRAPHS (not having 0 anymore)
*/

class Hypergraph {
    public:
        typedef std::set<unsigned> Hyperedges;

        Hypergraph();
        ~Hypergraph();

        /*Factory functions for member edges*/
        unsigned create(const std::string& label=""); // creates a new hyperedge
        unsigned create(Hyperedges edges, const std::string& label="");
        bool     create(const unsigned id, 
                        const std::string& label="");   // Tries to create a hyperedge with a given id ... if already taken, returns false
        void destroy(const unsigned id); // Will remove a hyperedge from this hypergraph (and also disconnect it from anybody)

        /*Get access to edges*/
        Hyperedge* get(const unsigned id);              // Finds a hyperedge by id
        // TODO: If we had a put method, we could get rid of pointers?
        Hyperedges find(const std::string& label="") const;   // Finds all hyperedges with a certain label

        /*Connect edges*/
        bool fromTo(const unsigned srcId, const unsigned destId);
        void disconnect(const unsigned id); // Disconnects edge from all other edges

        /*Traverse connected subgraphs*/
        enum TraversalDirection {
            DOWN,   // in direction of the _to set
            UP,     // in direction of the _from set
            BOTH    // in direction of both
        };
        template <typename ResultFilter, typename TraversalFilter> unsigned traversal(
            const unsigned rootId,              // The starting edge
            ResultFilter f,                         // Unary function bool f(Hyperedge *)
            TraversalFilter g,                      // Binary function bool g(Hyperedge *next, Hyperedge *current)
            const std::string& label="Traversal",   // Label for the result hyperedge
            const TraversalDirection dir = DOWN     // Direction of traversal
        );
        /* Merge operations on hyperedges*/
        // Unites the to sets of A and B creating a new edge C
        unsigned unite(const unsigned idA, const unsigned idB);
        // Intersect the to sets of A and B creating a new edge C
        unsigned intersect(const unsigned idA, const unsigned idB);
        // Create an edge C which contains all edges A points to but B does not point to
        unsigned subtract(const unsigned idA, const unsigned idB);

        /* TODO: We also need a possibility to check hypergraph equality*/
        /* TODO: We also need merge operations for complete hypergraphs!!!*/
        // Who gets the ownership of the edge pointers?
        /*template<typename EquivalenceRelation> Hypergraph* mergeWith(Hypergraph* other, EquivalenceRelation eqr);*/

        //TODO: Do these make any sense here?
        //Hyperedge* labelContains(const std::string& str="");
        //Hyperedge* labelPartOf(const std::string& str="");
        //Hyperedge* cardinalityLessThanOrEqual(const unsigned cardinality=0);
        //Hyperedge* cardinalityGreaterThan(const unsigned cardinality=0);
        //TODO: These kinda make sense in a hypergraph
        //Hyperedge* successors(); // Go in direction of to set and register all edges AND edge of edges
        //Hyperedge* predecessors(); // Go in direction of from set and register all edges AND edges of edges

    private:
        /*Traversal which does not construct a new edge. Useful for internal use in e.g. constructors*/
        template <typename ResultFilter, typename TraversalFilter> Hyperedges _traversal
        ( 
            const unsigned rootId,              // The starting edge
            ResultFilter f,
            TraversalFilter g,
            const TraversalDirection dir = DOWN
        );

        // Private members for factory
        unsigned _lastId;                      // this is the id we can safely assign but should increase whenever we used it
        // TODO: Do we need pointers anymore?
        std::map<unsigned, Hyperedge*> _edges; // stores all dynamically created hyperedges (the ones this hypergraph created!)
};

// Include template member functions
// See http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
#include "Hypergraph.tpp"

#endif
