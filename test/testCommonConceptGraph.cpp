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
    universe.create("Person");
    universe.create("Object");
    universe.create("Car");
    universe.isA(universe.find("Person"), universe.find("Object"));
    universe.isA(universe.find("Car"), universe.find("Object"));

    /* Create relation classes */
    universe.relate(universe.find("Person"), universe.find("Car"), "drive");
    universe.relate(universe.find("Person"), universe.find("Person"), "like");
    Hyperedges loveRelClassId = universe.relate(universe.find("Person"), universe.find("Person"), "love");
    loveRelClassId = subtract(loveRelClassId, universe.find("Person"));
    universe.subrelationOf(universe.relations("love"), universe.relations("like")); // If x loves y, x also likes y but not vice versa

    /* Create some persons and cars */
    universe.instantiateFrom(universe.find("Person"), "John");
    universe.instantiateFrom(universe.find("Person"), "Mary");
    universe.instantiateFrom(universe.find("Person"), "Alice");
    universe.instantiateFrom(universe.find("Person"), "Bob");
    universe.instantiateFrom(universe.find("Car"), "BMW");
    universe.instantiateFrom(universe.find("Car"), "VW T4");
    universe.instantiateFrom(universe.find("Car"), "Fiat Punto");

    /* Relate some people */
    universe.relateFrom(universe.instancesOf(universe.find("Person"), "Mary"), universe.instancesOf(universe.find("Person"), "John"), loveRelClassId);

    std::cout << "> Create a query for a person loving a person\n";
    /* Create a query */
    auto personA = universe.create("");
    auto personB = universe.create("");
    Hyperedges query;
    query = unite(query, universe.instanceOf(personA, universe.find("Person")));
    query = unite(query, universe.instanceOf(personB, universe.find("Person")));
    query = unite(query, universe.relate(personA, personB, "love"));
    std::cout << query << std::endl;
    Mapping mapping = universe.match(query);
    for (const auto &pair : mapping)
    {
        std::cout << *universe.get(pair.first) << " -> " << *universe.get(pair.second) << "\n";
    }

    std::cout << "> Find all matches for something loving something\n";
    std::vector< Mapping > previous;
    while ((mapping = universe.match(query, previous)).size())
    {
        std::cout << "\n";
        for (const auto &pair : mapping)
        {
            std::cout << *universe.get(pair.first) << " -> " << *universe.get(pair.second) << "\n";
        }
        previous.push_back(mapping);
    }

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
