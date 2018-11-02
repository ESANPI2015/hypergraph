#include "Hypergraph.hpp"
#include "HypergraphYAML.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>
#include <getopt.h>
#include <chrono>

static struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"all", no_argument, 0, 'a'},
    {0,0,0,0}
};

void usage (const char *myName)
{
    std::cout << "Query a hypergraph using another one\n";
    std::cout << "Usage:\n";
    std::cout << myName << " <yaml-file-in> <other-yaml-file>\n\n";
    std::cout << "Options:\n";
    std::cout << "--help\t" << "Show usage\n";
    std::cout << "--all\t" << "Finds all occurrences of query graph in data graph\n";
    std::cout << "\nExample:\n";
    std::cout << myName << " datagraph.yml querygraph.yml\n";
}

int main (int argc, char **argv)
{
    bool find_all = false;

    std::cout << "Query a data hypergraph using a query hypergraph and subgraph isomorphism algorithm\n";

    // Parse command line
    int c;
    while (1)
    {
        int option_index = 0;
        c = getopt_long(argc, argv, "h", long_options, &option_index);
        if (c == -1)
            break;

        switch (c)
        {
            case 'a':
                find_all = true;
                break;
            case 'h':
            case '?':
                break;
            default:
                std::cout << "W00t?!\n";
                return -1;
        }
    }

    if ((argc - optind) < 2)
    {
        usage(argv[0]);
        return -1;
    }
    const std::string& fileNameIn(argv[optind]);
    const std::string& fileNameIn2(argv[optind+1]);

    // Load graph
    Hypergraph datagraph(YAML::LoadFile(fileNameIn).as<Hypergraph>());
    Hypergraph querygraph(YAML::LoadFile(fileNameIn2).as<Hypergraph>());

    // Find matching(s)
    std::cout << "Searching ...\n";
    unsigned int no_matches = 0;
    std::stack< Mapping > searchSpace;
    Mapping mapping(datagraph.match(querygraph, searchSpace, Hypergraph::defaultMatchFunc));

    if (!mapping.size())
    {
        std::cout << "No match found\n";
        return 0;
    }
    no_matches++;

    // Dump first match
    for (const auto &pair : mapping)
    {
        std::cout << querygraph.read(pair.first) << " -> " << datagraph.read(pair.second) << "\n";
    }

    // Dump other matches if desired
    if (find_all)
    {
        while ((mapping = datagraph.match(querygraph, searchSpace, Hypergraph::defaultMatchFunc)).size())
        {
            std::cout << "\n";
            for (const auto &pair : mapping)
            {
                std::cout << querygraph.read(pair.first) << " -> " << datagraph.read(pair.second) << "\n";
            }
            no_matches++;
        }
    }

    std::cout << "\nFound " << no_matches << " matches\n";

    return no_matches;
}
