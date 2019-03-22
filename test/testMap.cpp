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
    universe.concept("TypeA", "TypeA");
    universe.concept("TypeB", "TypeB");

    // Then we need a mapping relation
    universe.relate("mappingRelation", universe.concepts("TypeA"), universe.concepts("TypeB"), "mappedTo");


    // Create some individuals of the two classes
    universe.instantiateFrom(universe.concepts("TypeA"), "A");
    universe.instantiateFrom(universe.concepts("TypeA"), "B");
    universe.instantiateFrom(universe.concepts("TypeA"), "C");

    universe.instantiateFrom(universe.concepts("TypeB"), "X");
    universe.instantiateFrom(universe.concepts("TypeB"), "Y");
    universe.instantiateFrom(universe.concepts("TypeB"), "Z");


    // Define mapping functions
    auto partitionFuncLeft = [] (const CommonConceptGraph& g) -> Hyperedges {
        return g.instancesOf("TypeA");
    };
    auto partitionFuncRight = [] (const CommonConceptGraph& g) -> Hyperedges {
        return g.instancesOf("TypeB");
    };

    auto matchFunc = [] (const CommonConceptGraph& g, const UniqueId& a, const UniqueId& b) -> float {
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

    auto mapFunc = [] (CommonConceptGraph& g, const UniqueId& a, const UniqueId& b) -> void {
        const std::string& labelA(g.access(a).label());
        const std::string& labelB(g.access(b).label());
        if ((labelA == "A") && (labelB == "X"))
            resources["X"] -= 0.1f;
        else if ((labelA == "A") && (labelB == "Y"))
            resources["Y"] -= 0.2f;
        else if ((labelA == "A") && (labelB == "Z"))
            resources["Z"] -= 0.3f;
        else if (labelA == "B")
            resources[labelB] -= 0.1f;
        else if ((labelA == "C") && (labelB == "Y"))
            resources["Y"] -= 0.5f;
        else
            resources[labelB] -= 1.0f;
        g.factFrom(Hyperedges{a}, Hyperedges{b}, "mappingRelation");
    };

    universe.importFrom(universe.map(partitionFuncLeft, partitionFuncRight, matchFunc, mapFunc));

    for (const UniqueId& a : universe.instancesOf("TypeA"))
    {
        Hyperedges otherUids(universe.isPointingTo(universe.factsOf("mappingRelation", Hyperedges{a})));
        for (const UniqueId& b : otherUids)
        {
            std::cout << universe.access(a).label() << " -> " << universe.access(b).label() << "\n";
            std::cout << "Resources left: " << resources[universe.access(b).label()] << "\n";
        }
    }

    return 0;
}
