#include "HypergraphDB.hpp"
#include "HypergraphYAML.hpp"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <fstream>
#include <iostream>
#include <cassert>

int main(void)
{
    std::cout << "*** HYPERGRAPH DB TEST ***" << std::endl;

    std::cout << "> Create DB\n";
    HypergraphDB myDB;

    std::cout << "> Load common universe\n";
    Hypergraph myGraph(YAML::LoadFile("commonUniverse.yml").as<Hypergraph>());

    std::cout << "> Commit to DB\n";
    myDB.commit("commonUniverse", myGraph);
    
    std::cout << "*** TESTS DONE ***" << std::endl;

    return 0;
}
