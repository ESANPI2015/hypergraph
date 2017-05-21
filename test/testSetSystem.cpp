#include "SetSystem.hpp"
#include "Set.hpp"
#include "HyperedgeYAML.hpp"

#include <fstream>
#include <iostream>
#include <cassert>

int main(void)
{
    std::cout << "*** SETSYSTEM TEST ***" << std::endl;

    SetSystem universe;

    std::cout << "> Create set" << std::endl;
    auto firstId = universe.create("First set");
    std::cout << "First id: " << firstId << "\n";

    std::cout << "> Create set with desired id" << std::endl;
    assert(universe.create(23, "Set with id 23") == true);

    std::cout << "> Create another set and check it" << std::endl;
    auto secondId = universe.create("Second set");
    std::cout << "Second id: " << secondId << "\n";
    assert(universe.isSet(secondId) == true);

    std::cout << "> Make second set member of first set\n";
    universe.get(secondId)->memberOf(&universe, firstId); // TODO: Looks nasty
    assert(universe.get(firstId)->members(&universe).count(secondId) == 1);

    std::cout << "> All sets" << std::endl;
    auto sets = universe.find();
    for (auto setId : sets)
    {
        std::cout << setId << " " << universe.get(setId)->label() << std::endl;
    }

    std::cout << "> Store setsystem using YAML" << std::endl;

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
        std::cout << edgeId << std::endl;
    }

    std::cout << "> Make it a setsystem" << std::endl;
    SetSystem *other = static_cast<SetSystem*>(restoredGraph); // Hmmm, maybe we should use dynamic_cast?
    auto superclassId = other->getSetClass();
    std::cout << "Id of superclass: " << superclassId << "\n";

    std::cout << "> All sets" << std::endl;
    sets = other->find();
    for (auto setId : sets)
    {
        std::cout << setId << std::endl;
    }

    std::cout << "*** TESTS DONE ***" << std::endl;

    return 0;
}
