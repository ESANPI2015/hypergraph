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
    universe.concept("PERSON", "Person");
    universe.concept("OBJECT", "Object");
    universe.concept("CAR", "Car");
    universe.isA(universe.concepts("Person"), universe.concepts("Object"));
    universe.isA(universe.concepts("Car"), universe.concepts("Object"));

    /* Create relation classes */
    universe.relate(universe.concepts("Person"), universe.concepts("Car"), "drive");
    universe.relate(universe.concepts("Person"), universe.concepts("Person"), "like");
    Hyperedges loveRelClassId = universe.relate(universe.concepts("Person"), universe.concepts("Person"), "love");
    loveRelClassId = subtract(loveRelClassId, universe.concepts("Person"));
    universe.subrelationOf(universe.relations("love"), universe.relations("like")); // If x loves y, x also likes y but not vice versa

    /* Create some persons and cars */
    std::cout << universe.instantiateFrom(universe.concepts("Person"), "John") << "\n";
    universe.instantiateFrom(universe.concepts("Person"), "Mary");
    universe.instantiateFrom(universe.concepts("Person"), "Alice");
    universe.instantiateFrom(universe.concepts("Person"), "Bob");
    universe.instantiateFrom(universe.concepts("Car"), "BMW");
    universe.instantiateFrom(universe.concepts("Car"), "VW T4");
    universe.instantiateFrom(universe.concepts("Car"), "Fiat Punto");

    /* Relate some people */
    std::cout << universe.concepts("Person") << "\n";
    std::cout << universe.instancesOf(universe.concepts("Person"), "Mary") << "\n";
    std::cout << universe.instancesOf(universe.concepts("Person"), "John") << "\n";
    std::cout << universe.factFrom(universe.instancesOf(universe.concepts("Person"), "Mary"), universe.instancesOf(universe.concepts("Person"), "John"), loveRelClassId) << "\n";
    universe.factFrom(universe.instancesOf(universe.concepts("Person"), "Alice"), universe.instancesOf(universe.concepts("Person"), "John"), loveRelClassId);

    std::cout << "> Create a query for a person loving another person\n";
    /* Create a query */
    CommonConceptGraph queryGraph;
    auto personA = queryGraph.concept("*","");
    auto personB = queryGraph.concept("**","");
    queryGraph.concept("personas", "Person");
    queryGraph.instanceOf(personA, queryGraph.concepts("Person"));
    queryGraph.instanceOf(personB, queryGraph.concepts("Person"));
    queryGraph.relate(queryGraph.concepts("Person"), queryGraph.concepts("Person"), "love");
    queryGraph.factFrom(personA, personB, queryGraph.relations("love"));
    /* Find query in unsiverse*/
    std::stack< Mapping > searchSpace;
    Mapping mapping = universe.match(queryGraph, searchSpace, Hypergraph::defaultMatchFunc);
    for (const auto &pair : mapping)
    {
        std::cout << queryGraph.access(pair.first) << " -> " << universe.access(pair.second) << "\n";
    }

    std::cout << "> Find all matches for some person loving another person\n";
    searchSpace = std::stack< Mapping >();
    while ((mapping = universe.match(queryGraph, searchSpace, Hypergraph::defaultMatchFunc)).size())
    {
        std::cout << "\n";
        for (const auto &pair : mapping)
        {
            std::cout << queryGraph.access(pair.first) << " -> " << universe.access(pair.second) << "\n";
        }
    }

    //std::cout << "> Create a replacement for a person loving another person\n";
    //CommonConceptGraph replacementGraph(queryGraph);
    //replacementGraph.relate(replacementGraph.concepts("Person"), replacementGraph.concepts("Person"), "like");
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
