#ifndef _HYPEREDGE_HPP
#define _HYPEREDGE_HPP

#include <vector>
#include <string>

class Hypergraph;
class Conceptgraph;

/*
* This is the main primitive of a generalized hypergraph: the hyperedge
* This hyperedge can point to other hyperedges.
* Note, that nodes are not needed anymore since they are special, 0-hyperedges.
* You can however derive them from 0-Hyperedges.
* NOTE: Only ids are stored ... pointers are avoided as much as possible
*/

typedef std::string UniqueId;
typedef std::vector<UniqueId> Hyperedges;
Hyperedges unite(const Hyperedges& a, const Hyperedges& b);       // Unites the two hyperedge sets to a new one (no duplicates!)
Hyperedges intersect(const Hyperedges& a, const Hyperedges& b);   // Intersects the two hyperedge sets and returns the result (DLOG)
Hyperedges subtract(const Hyperedges& a, const Hyperedges& b);    // Returns all edges which are in A but not in B
std::ostream& operator<< (std::ostream& os , const Hyperedges& val);

class Hyperedge
{
    friend class Hypergraph;
    friend class Conceptgraph;

    public:
        /*Constructor*/
        Hyperedge(const UniqueId& id="", const std::string& label=""); // NOTE: We need a default constructor with 0 arguments to use STL containers

        /*Destructor*/
        ~Hyperedge();

        /*Read Access*/
        const UniqueId& id() const; 
        const std::string& label() const; 
        const unsigned indegree() const;
        const unsigned outdegree() const;
        const Hyperedges& pointingTo() const;
        const Hyperedges& pointingFrom() const;
        const bool isPointingTo(const UniqueId& id) const;
        const bool isPointingFrom(const UniqueId& id) const;

        /*Write access*/
        void updateLabel(const std::string& label);
        void pointsFrom(const UniqueId& id);
        void pointsTo(const UniqueId& id);

        /* Serialization */
        friend std::ostream& operator<< (std::ostream& stream, const Hyperedge& edge);

        /*Graph dependent operations*/
        const bool isPartOf(Hypergraph &graph) const;

    protected:
        /*Private members*/
        UniqueId    _id;
        std::string _label;
        Hyperedges  _from;  // This is the row of an incidence matrix
        Hyperedges  _to;    // This is the column of an incidence matrix
        Hyperedges  _fromOthers; // This is a cache of other hyperedge ids pointing from us (when looking in here it has to be checked if valid!!!)
        Hyperedges  _toOthers;   // This is a cache of other hyperedge ids pointing to us
};

#endif
