#ifndef _HYPERGRAPH_DB_HPP
#define _HYPERGRAPH_DB_HPP

/*

    This class provides a simple API to effectively create a Hypergraph DB
    When created it initializes a GIT repository.
    For each new Hypergraph it will create a YAML file in the GIT named after the Hypergraph.
    When finished working with a Hypergraph, a call to commit will try to commit the changes into the GIT.

    NOTES:
    * Assign a unique id/name to hypergraph?
    * Shall we back up the Hypergraphs in a list and return references?
*/

#include "Hypergraph.hpp"
#include <git2.h>

class HypergraphDB {

    public:
        HypergraphDB(const std::string& dir="./HDB");
        ~HypergraphDB();

        Hypergraph open(const std::string& name);
        bool commit(const std::string& name, const Hypergraph& graph);
        //bool publish();
    protected:
        git_repository *repo;
};


#endif
