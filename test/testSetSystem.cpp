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
    auto id = universe.create("First set");
    assert(id == 2);
    std::cout << "First id: " << id << "\n";

    std::cout << "> Create set with desired id" << std::endl;
    assert(universe.create(23, "Set with id 23") == true);

    std::cout << "> Create another set and check it" << std::endl;
    id = universe.create("Second set");
    assert(id == 5);
    std::cout << "Second id: " << id << "\n";
    assert(universe.isSet(id) == true);

    std::cout << "> Make second set member of first set\n";
    universe.get(5)->memberOf(&universe, 2); // TODO: Looks nasty
    assert(universe.get(2)->members(&universe).count(5) == 1);

    std::cout << "> All sets" << std::endl;
    auto sets = universe.find();
    for (auto setId : sets)
    {
        std::cout << setId << std::endl;
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

    std::cout << "*** TESTS DONE ***" << std::endl;

    return 0;
}
