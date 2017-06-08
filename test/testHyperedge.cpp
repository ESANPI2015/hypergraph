#include "Hypergraph.hpp"
#include "Hyperedge.hpp"
#include "HyperedgeYAML.hpp"

#include <fstream>
#include <iostream>
#include <cassert>

int main(void)
{
    std::cout << "*** HYPERGRAPH TEST ***" << std::endl;

    Hypergraph testGraph;

    std::cout << "> Create edge" << std::endl;
    auto id = testGraph.create("First edge");
    assert(id == 1);
    std::cout << "First id: " << id << "\n";

    std::cout << "> Create edge with desired id" << std::endl;
    assert(testGraph.create(23, "Edge with id 23") == true);

    std::cout << "> Create edge" << std::endl;
    id = testGraph.create("Second edge");
    assert(id == 2);
    std::cout << "Second id: " << id << "\n";

    std::cout << "> All edges" << std::endl;
    auto edges = testGraph.find();
    for (auto edgeId : edges)
    {
        std::cout << edgeId << std::endl;
    }

    std::cout << "> Connect edge 1 -> 2" << std::endl;
    assert(testGraph.to(1,2) == true);
    std::cout << "> Connect edges 2 -> 23" << std::endl;
    assert(testGraph.to(2,23) == true);
    std::cout << "> Connect edges 23 -> 24 (should fail)" << std::endl;
    assert(testGraph.to(23,24) == false);
    std::cout << "> Connect edge 1 <- 23" << std::endl;
    assert(testGraph.from(23,1) == true);
    std::cout << "> Make a traversal starting at 1" << std::endl;
    id = testGraph.traversal(
            1,
            [](Hyperedge *x){ std::cout << *x << std::endl; return true; },
            [](Hyperedge *x, Hyperedge *y){ return true; },
            "MyTraversal"
         );
    assert(id == 3);
    // TODO: Missing: unite, intersect, subtract

    std::cout << "> Deleting traversal edge" << std::endl;
    testGraph.destroy(id);
    assert(testGraph.get(id) == NULL);

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
    edges = restoredGraph->find();
    for (auto edgeId : edges)
    {
        std::cout << edgeId << std::endl;
    }

    std::cout << "*** TESTS DONE ***" << std::endl;

    return 0;
}
