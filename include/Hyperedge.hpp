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

class Hyperedge
{
    friend class Hypergraph;

    public:
        typedef std::set<unsigned> Hyperedges;

        /*Constructor*/
        Hyperedge(const unsigned id, const std::string& label="");

        /*Destructor*/
        ~Hyperedge();

        /*Read Access*/
        unsigned id() const; 
        std::string label() const; 
        unsigned cardinality() const;
        Hyperedges pointingTo() const;
        Hyperedges pointedBy() const;
        bool isPointingTo(const unsigned id);
        bool isPointedBy(const unsigned id);

        /*Write access*/
        void updateLabel(const std::string& label);

        /* Serialization */
        friend std::ostream& operator<< (std::ostream& stream, const Hyperedge* edge);

        /*Graph dependent operations*/
        bool isPartOf(Hypergraph *graph);
        bool pointTo(Hypergraph *graph, const unsigned id);
        bool pointTo(Hypergraph *graph, Hyperedges otherIds);
        Hyperedges pointingTo(Hypergraph *graph, const std::string& label);
        Hyperedges pointedBy(Hypergraph *graph, const std::string& label);

    private:
        /*Private members*/
        unsigned    _id;
        std::string _label;
        Hyperedges  _from;  // This is the row of an incidence matrix
        Hyperedges  _to;    // This is the column of an incidence matrix
};

#endif
