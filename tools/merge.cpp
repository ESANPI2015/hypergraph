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
    std::cout << "Merge two graphs to become one\n";
    std::cout << "Usage:\n";
    std::cout << myName << " <yaml-file-in> <yaml-file-in> <yam-file-out>\n\n";
    std::cout << "Options:\n";
    std::cout << "--help\t" << "Show usage\n";
    std::cout << "\nExample:\n";
    std::cout << myName << " hg1.yml hg2.yml merged.yml\n";
}

int main (int argc, char **argv)
{
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
            case 'h':
            case '?':
                break;
            default:
                std::cout << "W00t?!\n";
                return 1;
        }
    }

    if ((argc - optind) < 3)
    {
        usage(argv[0]);
        return 1;
    }
    std::string fileNameIn1(argv[optind]);
    std::string fileNameIn2(argv[optind+1]);
    std::string fileNameOut(argv[optind+2]);

    // Load graphs and merge them using constructor
    Hypergraph hypergraph1(YAML::LoadFile(fileNameIn1).as<Hypergraph>());
    Hypergraph hypergraph2(YAML::LoadFile(fileNameIn2).as<Hypergraph>());
    Hypergraph merged(hypergraph1, hypergraph2);

    // Store graph
    fout.open(fileNameOut);
    if(fout.good()) {
        fout << YAML::StringFrom(merged) << std::endl;
    } else {
        std::cout << "FAILED\n";
    }
    fout.close();

    return 0;
}
