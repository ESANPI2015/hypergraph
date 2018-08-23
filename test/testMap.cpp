#include "CommonConceptGraph.hpp"
#include "HypergraphYAML.hpp"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <fstream>
#include <iostream>
#include <cassert>

// Each B shall have resources of 1.f initially
std::map< std::string, float > resources = {{"X", 1.f}, {"Y", 1.f}, {"Z", 1.f}};

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

    auto costFunc = [] (CommonConceptGraph& g, const UniqueId& a, const UniqueId& b) -> float {
        // Here we assign some arbitrary costs
        const std::string& labelA(g.get(a)->label());
        const std::string& labelB(g.get(b)->label());
        if ((labelA == "A") && (labelB == "X"))
            return resources["X"] - 0.1f;
        if ((labelA == "A") && (labelB == "Y"))
            return resources["Y"] - 0.2f;
        if ((labelA == "A") && (labelB == "Z"))
            return resources["Z"] - 0.3f;
        if (labelA == "B")
            return resources[labelB] - 0.1f;
        if ((labelA == "C") && (labelB == "Y"))
            return resources["Y"] - 0.5f;
        return resources[labelB] - 1.0f;
    };

    auto mapFunc = [] (CommonConceptGraph& g, const UniqueId& a, const UniqueId& b) -> void {
        const std::string& labelA(g.get(a)->label());
        const std::string& labelB(g.get(b)->label());
        if ((labelA == "A") && (labelB == "X"))
            resources["X"] -= 0.1f;
        if ((labelA == "A") && (labelB == "Y"))
            resources["Y"] -= 0.2f;
        if ((labelA == "A") && (labelB == "Z"))
            resources["Z"] -= 0.3f;
        if (labelA == "B")
            resources[labelB] -= 0.1f;
        if ((labelA == "C") && (labelB == "Y"))
            resources["Y"] -= 0.5f;
        resources[labelB] -= 1.0f;
    };

    universe.map(matchFunc, costFunc, mapFunc);

    return 0;
}
