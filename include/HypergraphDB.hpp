#ifndef _HYPERGRAPH_DB_HPP
#define _HYPERGRAPH_DB_HPP

/*

    This class provides a simple API to effectively create a Hypergraph DB
    When created, it creates a directory (if it doesn't exist yet)
    If connect() is called, either a new git repository is created or the exisiting one is updated (pull)
    With open(), one of the Hypergraphs is loaded and can be used.
    When finished with processing the Hypergraph, commit() takes care to update the local file and repository.
    Finally, when releasing the DB, publish() will merge the changes with the remote end and publish the changes.
*/

#include "Hypergraph.hpp"

class HypergraphDB {

    public:
        HypergraphDB(const std::string& dir="hypergraphDB", const std::string& dbName="", const std::string& dbUri="");
        ~HypergraphDB();

        const bool connect(const std::string& name, const std::string& uri);
        Hypergraph open(const std::string& name) const;
        const bool commit(const std::string& name, const Hypergraph& graph) const;
        const bool publish() const;
    protected:
        std::string localDir;
        std::string databaseName;
        std::string databaseUri;
};


#endif
