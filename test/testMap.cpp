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
    std::cout << "*** COMMON CONCEPT GRAPH MAPPING TEST ***" << std::endl;

    CommonConceptGraph universe;

    // At first we need two superclasses
    universe.create("TypeA", "TypeA");
    universe.create("TypeB", "TypeB");

    // Then we need a mapping relation
    universe.relate("mappingRelation", universe.find("TypeA"), universe.find("TypeB"), "mappedTo");


    // Create some individuals of the two classes
    universe.instantiateFrom(universe.find("TypeA"), "A");
    universe.instantiateFrom(universe.find("TypeA"), "B");
    universe.instantiateFrom(universe.find("TypeA"), "C");

    universe.instantiateFrom(universe.find("TypeB"), "X");
    universe.instantiateFrom(universe.find("TypeB"), "Y");
    universe.instantiateFrom(universe.find("TypeB"), "Z");


    // Define mapping functions
    auto matchFunc = [] (CommonConceptGraph& g, const UniqueId& a, const UniqueId& b) -> bool {
        // If a is not of type A, return false
        if (!g.instancesOf(Hyperedges{a}, "TypeA", CommonConceptGraph::TraversalDirection::FORWARD).size())
            return false;
        // If b is not of type B, return false
        if (!g.instancesOf(Hyperedges{b}, "TypeB", CommonConceptGraph::TraversalDirection::FORWARD).size())
            return false;
        return true;
    };

    auto resourceFunc = [] (CommonConceptGraph& g, const UniqueId& b) -> float {
        // Simple resource function for every target
        return 1.f;
    };

    auto costFunc = [] (CommonConceptGraph& g, const UniqueId& a, const UniqueId& b) -> float {
        // Here we assign some arbitrary costs
        const std::string& labelA(g.get(a)->label());
        const std::string& labelB(g.get(b)->label());
        if ((labelA == "A") && (labelB == "X"))
            return 0.1f;
        if ((labelA == "A") && (labelB == "Y"))
            return 0.2f;
        if ((labelA == "A") && (labelB == "Z"))
            return 0.3f;
        if (labelA == "B")
            return 0.1f;
        if ((labelA == "C") && (labelB == "Y"))
            return 0.5f;
        return 1.0f;
    };

    universe.map(matchFunc, resourceFunc, costFunc, "mappingRelation");

    return 0;
}
