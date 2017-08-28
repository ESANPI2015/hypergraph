#ifndef _HYPEREDGE_HPP
#define _HYPEREDGE_HPP

#include <set>
#include <string>

class Hypergraph;

/*
* This is the main primitive of a generalized hypergraph: the hyperedge
* This hyperedge can point to other hyperedges.
* Note, that nodes are not needed anymore since they are special, 0-hyperedges.
* You can however derive them from 0-Hyperedges.
* NOTE: Only ids are stored ... pointers are avoided as much as possible
*/

typedef std::set<unsigned> Hyperedges;
Hyperedges unite(const Hyperedges& a, const Hyperedges& b);       // Unites the two hyperedge sets to a new one
Hyperedges intersect(const Hyperedges& a, const Hyperedges& b);   // Intersects the two hyperedge sets and returns the result (DLOG)
Hyperedges subtract(const Hyperedges& a, const Hyperedges& b);    // Returns all edges which are in A but not in B
std::ostream& operator<< (std::ostream& os , const Hyperedges& val);

class Hyperedge
{
    friend class Hypergraph;

    public:
        /*Constructor*/
        Hyperedge(const unsigned id=0, const std::string& label=""); // NOTE: We need a default constructor with 0 arguments to use STL containers

        /*Destructor*/
        ~Hyperedge();

        /*Read Access*/
        unsigned id() const; 
        std::string label() const; 
        unsigned indegree() const;
        unsigned outdegree() const;
        Hyperedges pointingTo() const;
        Hyperedges pointingFrom() const;
        bool isPointingTo(const unsigned id);
        bool isPointingFrom(const unsigned id);

        /*Write access*/
        void updateLabel(const std::string& label);
        void from(const unsigned id);
        void to(const unsigned id);

        /* Serialization */
        friend std::ostream& operator<< (std::ostream& stream, const Hyperedge& edge);

        /*Graph dependent operations*/
        bool isPartOf(Hypergraph &graph);

    private:
        /*Private members*/
        unsigned    _id;
        std::string _label;
        Hyperedges  _from;  // This is the row of an incidence matrix
        Hyperedges  _to;    // This is the column of an incidence matrix
};

#endif
