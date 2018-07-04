#include "CommonConceptGraph.hpp"
#include "HypergraphYAML.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>
#include <getopt.h>
#include <chrono>

static struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"uid", required_argument, 0, 'u'},
    {"label", required_argument, 0, 'l'},
    {0,0,0,0}
};

void usage (const char *myName)
{
    std::cout << "Query a common concept graph\n";
    std::cout << "Usage:\n";
    std::cout << myName << " <yaml-file-in>\n\n";
    std::cout << "Options:\n";
    std::cout << "--help\t" << "Show usage\n";
    std::cout << "--uid=<uid>\t" << "Specify the hyperedge to be queried by UID\n";
    std::cout << "--label=<label>\t" << "Specify the hyperedge(s) to be queried by label\n";
    std::cout << "\nExample:\n";
    std::cout << myName << " hypergraph.yml\n";
}

int main (int argc, char **argv)
{
    UniqueId uid;
    std::string label;
    std::ofstream fout;

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
            case 'u':
                uid=std::string(optarg);
                break;
            case 'l':
                label=std::string(optarg);
                break;
            case 'h':
            case '?':
                break;
            default:
                std::cout << "W00t?!\n";
                return 1;
        }
    }

    if ((argc - optind) < 1)
    {
        usage(argv[0]);
        return 1;
    }
    std::string fileNameIn(argv[optind]);

    // Load graph
    Hypergraph hypergraph(YAML::LoadFile(fileNameIn).as<Hypergraph>());
    CommonConceptGraph ccgraph(hypergraph);

    Hyperedges candidateUids(ccgraph.find(label));
    if (!uid.empty())
        candidateUids = intersect(candidateUids, Hyperedges{uid});

    for (const UniqueId& candidateUid : candidateUids)
    {
        std::cout << *ccgraph.get(candidateUid) << std::endl;
    }

    return 0;
}
