#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "CommonConceptGraph.hpp"
#include "HypergraphYAML.hpp"

#include <iostream>

TEST_CASE("Construct a conceptual graph with fundamental relations", "[CommonConceptGraph]")
{
    CommonConceptGraph ccg;
    REQUIRE(ccg.exists(CommonConceptGraph::FactOfId) == true);
    REQUIRE(ccg.isPointingFrom(Hyperedges{CommonConceptGraph::FactOfId}) == Hyperedges{Conceptgraph::IsRelationId, CommonConceptGraph::FactOfId});
    REQUIRE(ccg.isPointingTo(Hyperedges{CommonConceptGraph::FactOfId}) == Hyperedges{Conceptgraph::IsRelationId, CommonConceptGraph::FactOfId});
    // subtyping
    REQUIRE(ccg.concept("PERSON","Person") == Hyperedges{"PERSON"});
    REQUIRE(ccg.concept("OBJECT","Object") == Hyperedges{"OBJECT"});
    REQUIRE(ccg.concept("CAR","Car") == Hyperedges{"CAR"});
    REQUIRE(ccg.isA(Hyperedges{"PERSON", "CAR"}, Hyperedges{"OBJECT"}).size() == 2);
    REQUIRE(subtract(ccg.subclassesOf(Hyperedges{"OBJECT"}), Hyperedges{"OBJECT", "PERSON", "CAR"}).empty() == true);
    // class-instance
    REQUIRE(ccg.instantiateFrom("PERSON", "Mary").size() == 1);
    REQUIRE(ccg.instancesOf(Hyperedges{"PERSON"}).size() == 1);
    REQUIRE(ccg.access(ccg.instancesOf(Hyperedges{"PERSON"})[0]).label() == "Mary");
    // parent-child
    REQUIRE(ccg.instantiateFrom("PERSON", "Jesus").size() == 1);
    REQUIRE(ccg.hasA(ccg.instancesOf(Hyperedges{"PERSON"}, "Mary"), ccg.instancesOf(Hyperedges{"PERSON"}, "Jesus")).size() == 1);
    REQUIRE(ccg.childrenOf(ccg.instancesOf(Hyperedges{"PERSON"}, "Mary"), "Jesus").size() == 1);
    // subrelations-facts and dependent queries
    ccg.relate("LOVES", Hyperedges{"PERSON"}, Hyperedges{"PERSON"}, "loves");
    ccg.relate("LIKES", Hyperedges{"PERSON"}, Hyperedges{"PERSON"}, "likes");
    ccg.subrelationOf("LOVES", "LIKES"); // if x loves y then x likes y
    ccg.factFrom(ccg.instancesOf(Hyperedges{"PERSON"}, "Mary"), ccg.instantiateFrom("PERSON", "Josef"), "LOVES");
    ccg.factFrom(ccg.instancesOf(Hyperedges{"PERSON"}, "Mary"), ccg.instancesOf(Hyperedges{"PERSON"}, "Jesus"), "LIKES");
    REQUIRE(ccg.relatedTo(ccg.instancesOf(Hyperedges{"PERSON"}, "Mary"), Hyperedges{"LOVES"}).size() == 1);
    REQUIRE(ccg.relatedTo(ccg.instancesOf(Hyperedges{"PERSON"}, "Mary"), Hyperedges{"LIKES"}).size() == 2);
    // TODO: Part-Whole
    // TODO: Connectivity
    // TODO: Test mapping
}

TEST_CASE("Map some concepts to others constructing a new conceptual graph", "[Mapping]")
{
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

    // Each B shall have resources of 1.f initially
    std::map< std::string, float > resources = {{"X", 1.f}, {"Y", 1.f}, {"Z", 1.f}};

    // Define mapping functions
    auto partitionFuncLeft = [] (const CommonConceptGraph& g) -> Hyperedges {
        return g.instancesOf("TypeA");
    };
    auto partitionFuncRight = [] (const CommonConceptGraph& g) -> Hyperedges {
        return g.instancesOf("TypeB");
    };

    auto matchFunc = [&] (const CommonConceptGraph& g, const UniqueId& a, const UniqueId& b) -> float {
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

    auto mapFunc = [&] (CommonConceptGraph& g, const UniqueId& a, const UniqueId& b) -> void {
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

    // Now check if each (A,B,C) has been mapped to one of (X,Y,Z)
    for (const UniqueId& src : universe.instancesOf(Hyperedges{"TypeA"}))
    {
        REQUIRE(universe.relatedTo(Hyperedges{src}, Hyperedges{"mappingRelation"}).size() == 1);
    }
}
