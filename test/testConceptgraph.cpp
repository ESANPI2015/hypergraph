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
    universe.create(3, "First concept");

    std::cout << "> All concepts" << std::endl;
    auto concepts = universe.find();
    for (auto conceptId : concepts)
    {
        std::cout << conceptId << " " << universe.get(conceptId)->label() << std::endl;
    }

    std::cout << "> Create another concept and check it" << std::endl;
    universe.create(4, "Second concept");
    assert(universe.find("Second concept").count(4) > 0);

    std::cout << "> Relate the first and the second concept\n";
    universe.relate(5, 3, 4, "relatedTo");

    std::cout << "> Create a tree of concepts related by a common relation\n";
    universe.create(6, "Root");
    universe.create(11,"I");
    universe.create(12,"You");
    universe.create(13,"It"); 
    universe.create(14,"Huh?");
    universe.create(15, "Plural");
    universe.create(20, "We"); 
    universe.create(21, "You");
    universe.create(22, "They");
    auto hashedId = universe.create("Doh?");

    universe.relate(7,  6, 11, "R");
    universe.relate(8,  6, 12, "R");
    universe.relate(9,  6, 13, "R");
    universe.relate(10, 6, 14, "A");
    universe.relate(16, 15, 20, "R");
    universe.relate(17, 15, 21, "R");
    universe.relate(18, 15, 22, "R");
    universe.relate(19, 15, hashedId, "B");
    universe.relate(24, 6, 15, "R");

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

    std::cout << "> Create another concept graph for inexact pattern matching\n";
    
    Conceptgraph query;
    query.relate(query.create("Root"),query.create(""),"A");
    concepts = query.Hypergraph::find();
    for (auto conceptId : concepts)
    {
        std::cout << "\t" << *query.get(conceptId) << std::endl;
    }

    test.reset();
    test = static_cast<Hypergraph*>(&query);
    fout.open("query.yml");
    if(fout.good()) {
        fout << test;
    } else {
        std::cout << "FAILED\n";
    }
    fout.close();

    std::cout << "> Try to find a match of the query graph in the data graph\n";
    Hypergraph::Hyperedges hedges = universe2.match(query);
    for (unsigned id : hedges)
    {
        std::cout << "\t" << *universe2.get(id) << std::endl;
    }

    std::cout << "*** TESTS DONE ***" << std::endl;

    return 0;
}
