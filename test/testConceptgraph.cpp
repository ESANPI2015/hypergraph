#include "Conceptgraph.hpp"
#include "HyperedgeYAML.hpp"

#include <fstream>
#include <iostream>
#include <cassert>

int main(void)
{
    std::cout << "*** CONCEPT GRAPH TEST ***" << std::endl;

    Conceptgraph universe;

    std::cout << "> Create concept" << std::endl;
    auto firstId = universe.create("First concept");
    std::cout << "First id: " << firstId << "\n";

    std::cout << "> All concepts" << std::endl;
    auto concepts = universe.find();
    for (auto conceptId : concepts)
    {
        std::cout << conceptId << " " << universe.get(conceptId)->label() << std::endl;
    }

    std::cout << "> Create another concept and check it" << std::endl;
    auto secondId = universe.create("Second concept");
    std::cout << "Second id: " << secondId << "\n";
    assert(universe.find("Second concept").count(secondId) > 0);

    std::cout << "> Relate the first and the second concept\n";
    universe.relate(firstId, secondId, "relatedTo");

    std::cout << "> Create a tree of concepts related by a common relation\n";
    unsigned rootId = universe.create("Root");
    universe.relate(rootId, universe.create("I"), "R");
    universe.relate(rootId, universe.create("You"), "R");
    universe.relate(rootId, universe.create("It"), "R");
    universe.relate(rootId, universe.create("Huh?"), "A");
    unsigned pluralId = universe.create("Plural");
    universe.relate(pluralId, universe.create("We"), "R");
    universe.relate(pluralId, universe.create("You"), "R");
    universe.relate(pluralId, universe.create("They"), "R");
    universe.relate(pluralId, universe.create("Doh?"), "B");
    universe.relate(rootId, pluralId, "R");

    std::cout << "> All concepts" << std::endl;
    concepts = universe.find();
    for (auto conceptId : concepts)
    {
        std::cout << conceptId << " " << universe.get(conceptId)->label() << std::endl;
    }

    std::cout << "> Store concept graph using YAML" << std::endl;

    YAML::Node test;
    test = static_cast<Hypergraph*>(&universe);

    std::ofstream fout;
    fout.open("universe.yml");
    if(fout.good()) {
        fout << test;
    } else {
        std::cout << "FAILED\n";
    }
    fout.close();

    std::cout << "> Create new hypergraph from YAML" << std::endl;

    test.reset();
    test = YAML::LoadFile("universe.yml");
    Hypergraph *restoredGraph = test.as<Hypergraph*>();

    std::cout << "> All edges of restored graph" << std::endl;
    auto edges = restoredGraph->find();
    for (auto edgeId : edges)
    {
        std::cout << *restoredGraph->get(edgeId) << std::endl;
    }

    std::cout << "> Make it a concept graph" << std::endl;
    Conceptgraph universe2(*restoredGraph);

    std::cout << "> All concepts" << std::endl;
    concepts = universe2.find();
    for (auto conceptId : concepts)
    {
        std::cout << conceptId << " " << universe2.get(conceptId)->label() << std::endl;
        auto relations = universe2.relationsOf(conceptId);
        for (auto relId : relations)
        {
            std::cout << "\t" << relId << " " << universe2.get(relId)->label() << std::endl;
        }
    }

    std::cout << "> Make a traversal returning concepts connected by a certain relation\n";
    concepts = universe2.traverse(*universe2.find("Root").begin(), "", "R");
    for (auto conceptId : concepts)
    {
        std::cout << conceptId << " " << universe2.get(conceptId)->label() << std::endl;
        auto relations = universe2.relationsOf(conceptId);
        for (auto relId : relations)
        {
            std::cout << "\t" << relId << " " << universe2.get(relId)->label() << std::endl;
        }
    }

    std::cout << "*** TESTS DONE ***" << std::endl;

    return 0;
}