#ifndef _HYPEREDGE_HPP
#define _HYPEREDGE_HPP

#include <vector>
#include <unordered_map>
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

using UniqueId = std::string;                                       // Type used for unique ids of hyperedges
using Properties = std::unordered_map< std::string, std::string >;  // Type used to store and retrieve hyperedge properties
using Hyperedges = std::vector<UniqueId>;                           // Type used to store unique ids of a list of hyperedges

/* Operations on sets of Hyperedge UniqueIds */
Hyperedges unite(const Hyperedges& a, const Hyperedges& b);         // Unites the two hyperedge sets to a new one (no duplicates!)
Hyperedges intersect(const Hyperedges& a, const Hyperedges& b);     // Intersects the two hyperedge sets and returns the result (DLOG)
Hyperedges subtract(const Hyperedges& a, const Hyperedges& b);      // Returns all edges which are in A but not in B
std::ostream& operator<< (std::ostream& os , const Hyperedges& val);// Streaming operator to dump out a set of hyperedge unique ids

class Hyperedge
{
    friend class Hypergraph;
    friend class Conceptgraph;

    public:
        /*Constructor*/
        Hyperedge(const UniqueId& id="", const std::string& label="", const Properties& props=Properties()); // NOTE: We need a default constructor with 0 arguments to use STL containers (and only one)

        /*Destructor*/
        ~Hyperedge();

        /*Read Access*/
        const UniqueId& id() const; 
        const Properties& properties() const; 
        const std::string& property(const std::string& key) const;
        bool hasProperty(const std::string& key) const;
        const std::string& label() const
        {
            return property("label");
        }
        unsigned indegree() const;
        unsigned outdegree() const;
        const Hyperedges& pointingTo() const;
        const Hyperedges& pointingFrom() const;
        bool isPointingTo(const UniqueId& id) const;
        bool isPointingFrom(const UniqueId& id) const;

        /*Write access*/
        void property(const std::string& key, const std::string& val);
        void updateLabel(const std::string& value)
        {
            property("label", value);
        }
        void pointsFrom(const UniqueId& id);
        void pointsTo(const UniqueId& id);

        /* Serialization */
        friend std::ostream& operator<< (std::ostream& stream, const Hyperedge& edge);

    protected:
        /*Private members*/
        UniqueId    _id;
        Properties  _properties; // This stores any properties set on the hyperedge
        Hyperedges  _from;       // This is the row of an incidence matrix
        Hyperedges  _to;         // This is the column of an incidence matrix
        Hyperedges  _fromOthers; // This is a cache of other hyperedge ids pointing from us (when looking in here it has to be checked if valid!!!)
        Hyperedges  _toOthers;   // This is a cache of other hyperedge ids pointing to us
};

#endif
