#include "CommonConceptGraph.hpp"
#include "HypergraphYAML.hpp"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <fstream>
#include <iostream>
#include <cassert>

int main(void)
{
    std::cout << "*** COMMON CONCEPT GRAPH TEST ***" << std::endl;

    CommonConceptGraph universe;

    std::cout << "> Store empty common concept graph\n";
    std::ofstream fout;
    fout.open("emptyCCG.yml");
    if(fout.good()) {
        fout << YAML::StringFrom(universe) << std::endl;
    } else {
        std::cout << "FAILED\n";
    }
    fout.close();

    std::cout << "> Create a common concept graph\n";

    /* Create classes */
    universe.create("PERSON", "Person");
    universe.create("OBJECT", "Object");
    universe.create("CAR", "Car");
    universe.isA(universe.find("Person"), universe.find("Object"));
    universe.isA(universe.find("Car"), universe.find("Object"));

    /* Create relation classes */
    universe.relate(universe.find("Person"), universe.find("Car"), "drive");
    universe.relate(universe.find("Person"), universe.find("Person"), "like");
    Hyperedges loveRelClassId = universe.relate(universe.find("Person"), universe.find("Person"), "love");
    loveRelClassId = subtract(loveRelClassId, universe.find("Person"));
    universe.subrelationOf(universe.relations("love"), universe.relations("like")); // If x loves y, x also likes y but not vice versa

    /* Create some persons and cars */
    std::cout << universe.instantiateFrom(universe.find("Person"), "John") << "\n";
    universe.instantiateFrom(universe.find("Person"), "Mary");
    universe.instantiateFrom(universe.find("Person"), "Alice");
    universe.instantiateFrom(universe.find("Person"), "Bob");
    universe.instantiateFrom(universe.find("Car"), "BMW");
    universe.instantiateFrom(universe.find("Car"), "VW T4");
    universe.instantiateFrom(universe.find("Car"), "Fiat Punto");

    /* Relate some people */
    std::cout << universe.find("Person") << "\n";
    std::cout << universe.instancesOf(universe.find("Person"), "Mary") << "\n";
    std::cout << universe.instancesOf(universe.find("Person"), "John") << "\n";
    std::cout << universe.factFrom(universe.instancesOf(universe.find("Person"), "Mary"), universe.instancesOf(universe.find("Person"), "John"), loveRelClassId) << "\n";
    universe.factFrom(universe.instancesOf(universe.find("Person"), "Alice"), universe.instancesOf(universe.find("Person"), "John"), loveRelClassId);

    std::cout << "> Create a query for a person loving another person\n";
    /* Create a query */
    CommonConceptGraph queryGraph;
    auto personA = queryGraph.create("*","");
    auto personB = queryGraph.create("**","");
    queryGraph.create("personas", "Person");
    queryGraph.instanceOf(personA, queryGraph.find("Person"));
    queryGraph.instanceOf(personB, queryGraph.find("Person"));
    queryGraph.relate(queryGraph.find("Person"), queryGraph.find("Person"), "love");
    queryGraph.factFrom(personA, personB, queryGraph.relations("love"));
    /* Find query in unsiverse*/
    std::stack< Mapping > searchSpace;
    Mapping mapping = universe.match(queryGraph, searchSpace);
    for (const auto &pair : mapping)
    {
        std::cout << *queryGraph.get(pair.first) << " -> " << *universe.get(pair.second) << "\n";
    }

    std::cout << "> Find all matches for some person loving another person\n";
    searchSpace = std::stack< Mapping >();
    while ((mapping = universe.match(queryGraph, searchSpace)).size())
    {
        std::cout << "\n";
        for (const auto &pair : mapping)
        {
            std::cout << *queryGraph.get(pair.first) << " -> " << *universe.get(pair.second) << "\n";
        }
    }

    //std::cout << "> Create a replacement for a person loving another person\n";
    //CommonConceptGraph replacementGraph(queryGraph);
    //replacementGraph.relate(replacementGraph.find("Person"), replacementGraph.find("Person"), "like");
    //replacementGraph.factFrom(personA, personB, replacementGraph.relations("like"));
    // TODO: The replacement graph is now the query graph PLUS some additional nodes! These have to be added although they are not in the replacement Mapping
    // This has to be added somehow to the rewrite algorithm

    fout.open("commonUniverse.yml");
    if(fout.good()) {
        fout << YAML::StringFrom(universe) << std::endl;
    } else {
        std::cout << "FAILED\n";
    }
    fout.close();

    std::cout << "*** TESTS DONE ***" << std::endl;

    return 0;
}
