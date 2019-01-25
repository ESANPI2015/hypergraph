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
    auto partitionFuncLeft = [] (const CommonConceptGraph& g) -> Hyperedges {
        return g.instancesOf("TypeA");
    };
    auto partitionFuncRight = [] (const CommonConceptGraph& g) -> Hyperedges {
        return g.instancesOf("TypeB");
    };

    auto matchFunc = [] (const CommonConceptGraph& g, const UniqueId& a, const UniqueId& b) -> bool {
        // The partition function already did a good job :)
        return true;
    };

    auto costFunc = [] (const CommonConceptGraph& g, const UniqueId& a, const UniqueId& b) -> float {
        // Here we assign some arbitrary costs
        const std::string& labelA(g.access(a).label());
        const std::string& labelB(g.access(b).label());
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

    auto mapFunc = [] (const CommonConceptGraph& g, const UniqueId& a, const UniqueId& b) -> void {
        const std::string& labelA(g.access(a).label());
        const std::string& labelB(g.access(b).label());
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

    universe.map(partitionFuncLeft, partitionFuncRight, matchFunc, costFunc, mapFunc);

    return 0;
}
