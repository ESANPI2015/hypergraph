#include "HypergraphDB.hpp"
#include "HypergraphYAML.hpp"
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>

HypergraphDB::HypergraphDB(const std::string& dir)
: directory(dir)
{
    // Create an empty git repo if it does not exist
    int ret = std::system(("/bin/bash -c \"git init "+dir+"\"").c_str());
    std::cout << "GIT INIT: " << ret << "\n";
}

HypergraphDB::~HypergraphDB()
{
}

bool HypergraphDB::connect(const std::string& uri)
{
    // Store parameters
    dbUri = uri;
    // Add remote (if it exists, dont do anything)
    int ret = std::system(("/bin/bash -c \""+directory+"/git remote add hypergraphdb "+dbUri+"\"").c_str());
    // Pull from remote
    ret |= std::system((directory+"/git pull hypergraphdb master:master").c_str());
    return ret > 0 ? false : true;
}

Hypergraph HypergraphDB::load(const std::string& name)
{
    return YAML::LoadFile(directory+"/"+name).as<Hypergraph>();
}

bool HypergraphDB::commit(const std::string& name, const Hypergraph& graph)
{
    // Store the graph in directory
    std::ofstream hg;
    std::string filename(std::string(directory+"/"+name));
    hg.open(filename);
    if (!hg.good())
    {
        std::cout << "Failed to open " << filename << std::endl;
        return false;
    }
    hg << YAML::StringFrom(graph) << std::endl;
    hg.close();

    // Create a new GIT commit
    int ret = std::system(("/bin/bash -c \"cd "+directory+"; git add "+name+"\"").c_str());
    ret |= std::system(("/bin/bash -c \"cd "+directory+"; git commit -m \'"+name+"\'\"").c_str());
    return ret > 0 ? false : true;
}

bool HypergraphDB::publish()
{
    // We have a local master and a remote master, so we should merge
    int ret = std::system((directory+"/git pull hypergraphdb master:master").c_str());
    // Afterwards, we push to remote
    ret |= std::system((directory+"/git push hypergraphdb master:master").c_str());
    return ret > 0 ? false : true;
}
