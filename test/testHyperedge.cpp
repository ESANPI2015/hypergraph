#include "Hypergraph.hpp"
#include "Hyperedge.hpp"
#include "HyperedgeYAML.hpp"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <fstream>
#include <iostream>
#include <cassert>

int main(void)
{
    std::cout << "*** HYPERGRAPH TEST ***" << std::endl;

    Hypergraph testGraph;

    std::cout << "> Create edge" << std::endl;
    assert(testGraph.create("1", "First edge").empty() == false);

    std::cout << "> Create edge with desired id" << std::endl;
    assert(testGraph.create("23", "Edge with id 23").empty() == false);

    std::cout << "> Create edge" << std::endl;
    assert(testGraph.create("2", "Second edge").empty() == false);

    std::cout << "> All edges" << std::endl;
    std::cout << testGraph.find() << std::endl;

    std::cout << "> Connect edge 1 -> 2" << std::endl;
    assert(testGraph.to(Hyperedges{"1"},Hyperedges{"2"}).size() == 2);
    std::cout << "> Connect edges 2 -> 23" << std::endl;
    assert(testGraph.to(Hyperedges{"2"},Hyperedges{"23"}).size() == 2);
    std::cout << "> Connect edges 23 -> 24 (should fail)" << std::endl;
    assert(testGraph.to(Hyperedges{"23"},Hyperedges{"24"}).size() == 0);
    std::cout << "> Connect edge 23 <- 1" << std::endl;
    assert(testGraph.from(Hyperedges{"23"},Hyperedges{"1"}).size() == 2);
    std::cout << "> Make a traversal starting at 1" << std::endl;
    testGraph.traversal(
            "1",
            [](Hyperedge *x){ std::cout << *x << std::endl; return true; },
            [](Hyperedge *x, Hyperedge *y){ return true; }
         );

    std::cout << "> Store hypergraph using YAML" << std::endl;

    YAML::Node test;
    test = &testGraph;

    std::ofstream fout;
    fout.open("test.yml");
    if(fout.good()) {
        fout << test;
    } else {
        std::cout << "FAILED\n";
    }
    fout.close();

    std::cout << "> Create new hypergraph from YAML" << std::endl;

    test.reset();
    test = YAML::LoadFile("test.yml");
    Hypergraph *restoredGraph = test.as<Hypergraph*>();

    std::cout << "> All edges of restored graph" << std::endl;
    std::cout << restoredGraph->find() << std::endl;

    std::cout << "*** TESTS DONE ***" << std::endl;

    return 0;
}
