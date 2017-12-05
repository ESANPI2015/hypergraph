#include "CommonConceptGraph.hpp"
#include "HyperedgeYAML.hpp"

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
    std::cout << "Simplifies common concept graphs using graph rewriting\n";
    std::cout << "Usage:\n";
    std::cout << myName << " <yaml-file-in> <yam-file-out>\n\n";
    std::cout << "Options:\n";
    std::cout << "--help\t" << "Show usage\n";
    std::cout << "--all\t" << "Process all matches\n";
    std::cout << "\nExample:\n";
    std::cout << myName << " original.yml simplified.yml\n";
}

int main (int argc, char **argv)
{
    YAML::Node doc;
    std::ofstream fout;

    // Parse command line
    int c;
    bool processAll = false;
    while (1)
    {
        int option_index = 0;
        c = getopt_long(argc, argv, "h", long_options, &option_index);
        if (c == -1)
            break;

        switch (c)
        {
            case 'a':
                processAll = true;
                break;
            case 'h':
            case '?':
                break;
            default:
                std::cout << "W00t?!\n";
                return 1;
        }
    }

    if ((argc - optind) < 2)
    {
        usage(argv[0]);
        return 1;
    }
    std::string fileNameIn(argv[optind]);
    std::string fileNameOut(argv[optind+1]);

    // Load graph
    Hypergraph* hypergraph = YAML::LoadFile(fileNameIn).as<Hypergraph*>();
    Conceptgraph cgraph(*hypergraph);
    CommonConceptGraph ccgraph(cgraph);

    // The following rule merges two FACT-OF relations to the same RELATION CLASS into one FACT-OF

    // Create left hand side
    Conceptgraph lhs;
    lhs.relate("SomeRelation",Hyperedges{},Hyperedges{},"");
    lhs.relate("FirstFact", Hyperedges{}, Hyperedges{},"");
    lhs.relate("SecondFact", Hyperedges{}, Hyperedges{},"");
    Hyperedges factOf1 = subtract(lhs.relate("FACT-OF1", Hyperedges{"FirstFact"}, Hyperedges{"SomeRelation"}, "FACT-OF"), Hyperedges{"FirstFact","SomeRelation"});
    Hyperedges factOf2 = subtract(lhs.relate("FACT-OF2", Hyperedges{"SecondFact"}, Hyperedges{"SomeRelation"}, "FACT-OF"), Hyperedges{"SecondFact","SomeRelation"});
    std::cout << lhs.Hypergraph::find() << std::endl;

    // Create right hand side
    Conceptgraph rhs;
    rhs.relate("SomeRelation",Hyperedges{},Hyperedges{},"");
    rhs.relate("FirstFact", Hyperedges{}, Hyperedges{},"");
    rhs.relate("SecondFact", Hyperedges{}, Hyperedges{},"");
    Hyperedges factOf3 = subtract(rhs.relate("FACT-OF1", Hyperedges{"FirstFact","SecondFact"}, Hyperedges{"SomeRelation"}, "FACT-OF"), Hyperedges{"FirstFact","SecondFact","SomeRelation"});
    std::cout << rhs.Hypergraph::find() << std::endl;

    // Create partial homomorphism
    Mapping partial(fromHyperedges(lhs.Hypergraph::find()));
    partial[*factOf1.begin()] = *factOf3.begin();
    partial[*factOf2.begin()] = *factOf3.begin();
    std::cout << partial << std::endl;

    // Rewrite
    std::stack< Mapping > sp;
    std::chrono::high_resolution_clock::time_point start(std::chrono::high_resolution_clock::now());
    Hypergraph simplified(ccgraph.rewrite(lhs,rhs,partial,sp));
    if (!simplified.size())
    {
        std::cout << "No simplification possible\n";
        return 1;
    }
    std::cout << "." << std::flush;
    while (processAll)
    {
        // Rewrite
        Hypergraph simplified2 = simplified.rewrite(lhs,rhs,partial,sp);
        std::cout << "." << std::flush;
        if (simplified2.size())
        {
            simplified = simplified2;
        } else {
            break;
        }
        // Store graph (such that rewrite can be aborted)
        doc.reset();
        doc = static_cast<Hypergraph*>(&simplified);
        fout.open(fileNameOut);
        if(fout.good()) {
            fout << doc;
        } else {
            std::cout << "FAILED\n";
        }
        fout.close();
    }
    std::chrono::high_resolution_clock::time_point end(std::chrono::high_resolution_clock::now());
    std::cout << "Done.\n";
    std::cout << "Time elapsed [us]: " << std::chrono::duration_cast<std::chrono::microseconds>(end-start).count() << "\n";

    // Store graph
    doc.reset();
    doc = static_cast<Hypergraph*>(&simplified);
    fout.open(fileNameOut);
    if(fout.good()) {
        fout << doc;
    } else {
        std::cout << "FAILED\n";
    }
    fout.close();

    return 0;
}
