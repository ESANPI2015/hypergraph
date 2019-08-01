#include "HypergraphDB.hpp"
#include "HypergraphYAML.hpp"
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>

HypergraphDB::HypergraphDB(const std::string& dir, const std::string& dbName, const std::string& dbUri)
: localDir(dir), databaseName(dbName), databaseUri(dbUri)
{
    // Create an empty git repo if it does not exist
    int ret = std::system(("/bin/bash -c \"git init "+dir+"\"").c_str());
    if (ret)
    {
        std::cout << "HypergraphDB::HypergraphDB(): Failed to initialize git with errorcode: " << ret << std::endl;
        return;
    }
    if (dbName.empty() || dbUri.empty())
        return;
    connect(dbName, dbUri);
}

HypergraphDB::~HypergraphDB()
{
    // On destruction, perform an auto-publish (if possible)
    if (databaseName.empty() || databaseUri.empty())
        return;
    publish();
}

const bool HypergraphDB::connect(const std::string& name, const std::string& uri)
{
    // Store parameters
    databaseUri = uri;
    databaseName = name;
    // Add remote (if it exists, dont do anything)
    int ret = std::system(("/bin/bash -c \"cd "+localDir+"; git remote add "+databaseName+" "+databaseUri+"\"").c_str());
    // If it exists, we have to remove the old remote and recreate it
    if (ret > 0)
    {
        std::cout << "HypergraphDB::connect(): Remote " << databaseName << " exists. Will update URI\n";
        ret = std::system(("/bin/bash -c \"cd "+localDir+"; git remote rm "+databaseName+"\"").c_str());
        if (ret > 0)
        {
            std::cout << "HypergraphDB::connect(): Could not remove " << databaseName << "\n";
            return false;
        }
        ret = std::system(("/bin/bash -c \"cd "+localDir+"; git remote add "+databaseName+" "+databaseUri+"\"").c_str());
        if (ret > 0)
        {
            std::cout << "HypergraphDB::connect(): Could not re-add " << databaseName << "\n";
            return false;
        }
    }
    // Pull from remote
    ret = std::system(("/bin/bash -c \"cd "+localDir+"; git pull --rebase "+databaseName+" master:master").c_str());
    if (ret > 0)
    {
        std::cout << "HypergraphDB::connect(): Could not pull from " << databaseUri << "\n";
        return false;
    }
    return true;
}

Hypergraph HypergraphDB::open(const std::string& name) const
{
    // TODO: Check if file exists
    // If not, create empty Hypergraph, commit and open
    // If it does, open
    return YAML::LoadFile(localDir+"/"+name).as<Hypergraph>();
}

const bool HypergraphDB::commit(const std::string& name, const Hypergraph& graph) const
{
    // Store the graph in localDir
    std::ofstream hg;
    std::string filename(std::string(localDir+"/"+name));
    hg.open(filename);
    if (!hg.good())
    {
        std::cout << "HypergraphDB::commit(): Failed to open " << filename << std::endl;
        return false;
    }
    hg << YAML::StringFrom(graph) << std::endl;
    hg.close();

    // Create a new GIT commit
    int ret = std::system(("/bin/bash -c \"cd "+localDir+"; git add "+name+"\"").c_str());
    if (ret > 0)
    {
        std::cout << "HypergraphDB::commit(): Could not add "+name+"\n";
        return false;
    }
    ret = std::system(("/bin/bash -c \"cd "+localDir+"; git commit -m \'Update of "+name+"\'\"").c_str());
    if (ret > 0)
    {
        std::cout << "HypergraphDB::commit(): Could not commit "+name+"\n";
        return false;
    }
    return true;
}

const bool HypergraphDB::publish() const
{
    // Check
    if (databaseName.empty() || databaseUri.empty())
    {
        std::cout << "HypergraphDB::publish(): Not connected.\n";
        return false;
    }
    // We have a local master and a remote master, so we should merge
    int ret = std::system(("/bin/bash -c \"cd "+localDir+"; git pull --rebase "+databaseName+" master:master").c_str());
    if (ret > 0)
    {
        std::cout << "HypergraphDB::publish(): Could not pull from " << databaseUri << "\n";
        return false;
    }
    // Afterwards, we push to remote
    ret = std::system(("/bin/bash -c \"cd "+localDir+"; git push "+databaseName+" master:master").c_str());
    if (ret > 0)
    {
        std::cout << "HypergraphDB::publish(): Could not push to " << databaseUri << "\n";
        return false;
    }
    return true;
}
