#include "CommonConceptGraph.hpp"
#include "HyperedgeYAML.hpp"

#include <fstream>
#include <iostream>
#include <cassert>

int main(void)
{
    YAML::Node test;
    std::ofstream fout;
    std::cout << "*** COMMON CONCEPT GRAPH TEST ***" << std::endl;

    CommonConceptGraph universe;

    std::cout << "> Create a common concept graph\n";

    /* Create classes */
    unsigned personClassId = universe.create("Person");
    universe.create("Object");
    unsigned carClassId = universe.create("Car");
    universe.isA(universe.find("Person"), universe.find("Object"));
    universe.isA(universe.find("Car"), universe.find("Object"));

    /* Create relation classes */
    //unsigned driveClassId = 
    universe.relate(universe.find("Person"), universe.find("Car"), "drive");
    //unsigned likeClassId = 
    universe.relate(universe.find("Person"), universe.find("Person"), "like");
    unsigned loveRelClassId = universe.relate(universe.find("Person"), universe.find("Person"), "love");
    universe.subrelationOf(universe.relations("love"), universe.relations("like")); // If x loves y, x also likes y but not vice versa

    /* Create some persons and cars */
    universe.instantiateFrom(universe.first(universe.find("Person")), "John");
    universe.instantiateFrom(personClassId, "Mary");
    universe.instantiateFrom(personClassId, "Alice");
    universe.instantiateFrom(personClassId, "Bob");

    universe.instantiateFrom(carClassId, "BMW");
    universe.instantiateFrom(carClassId, "VW T4");
    universe.instantiateFrom(carClassId, "Fiat Punto");

    /* Relate some people */
    universe.relateFrom(universe.instancesOf(universe.find("Person"), "Mary"), universe.instancesOf(universe.find("Person"), "John"), loveRelClassId);

    test = static_cast<Hypergraph*>(&universe);
    fout.open("commonUniverse.yml");
    if(fout.good()) {
        fout << test;
    } else {
        std::cout << "FAILED\n";
    }
    fout.close();

    std::cout << "*** TESTS DONE ***" << std::endl;

    return 0;
}
