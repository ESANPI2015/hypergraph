#ifndef _HYPERGRAPH_DB_HPP
#define _HYPERGRAPH_DB_HPP

/*

    This class provides a simple API to effectively create a Hypergraph DB
    When created it initializes a GIT repository.
    For each new Hypergraph it will create a YAML file in the GIT named after the Hypergraph.
    When finished working with a Hypergraph, a call to commit will try to commit the changes into the GIT.

    TODO:
    * Every instance of this class should have their own branch (unique!)
    * Before starting to work, pull from remote master branch
    * On destruction, merge with master branch & push to remote

    NOTES:
    * Assign a unique id/name to hypergraph?
    * Shall we back up the Hypergraphs in a list and return references?
    * Instead of using libgit ... should we just use CLI interface of GIT?
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
