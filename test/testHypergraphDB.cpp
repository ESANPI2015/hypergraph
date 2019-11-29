#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "CommonConceptGraph.hpp"
#include "HypergraphDB.hpp"
#include "HypergraphYAML.hpp"

TEST_CASE("Create and update a small, local HypergraphDB", "[HypergraphDB]")
{
    HypergraphDB myDB;

    CommonConceptGraph universe;
    universe.concept("PERSON", "Person");
    universe.relate("WORKS-WITH", Hyperedges{"PERSON"}, Hyperedges{"PERSON"}, "works-with");
    universe.instantiateFrom("PERSON", "Moritz");
    universe.instantiateFrom("PERSON", "his colleague");
    universe.factFrom(universe.instancesOf(Hyperedges{"PERSON"}, "Moritz"), universe.instancesOf(Hyperedges{"PERSON"}, "his colleague"), "WORKS-WITH");
    REQUIRE(universe.relatedTo(universe.instancesOf(Hyperedges{"PERSON"}), Hyperedges{"WORKS-WITH"}).size() == 1);

    REQUIRE(myDB.commit("myUniverse", universe) == true);
    CommonConceptGraph otherUniverse(myDB.open("myUniverse"));
    // Check that graphs are equal
    for (const UniqueId& a : universe.findByLabel())
    {
        REQUIRE(otherUniverse.exists(a) == true);
        REQUIRE(universe.isPointingFrom(Hyperedges{a}) == otherUniverse.isPointingFrom(Hyperedges{a}));
        REQUIRE(universe.isPointingTo(Hyperedges{a}) == otherUniverse.isPointingTo(Hyperedges{a}));
    }
    Hyperedges colleagues(otherUniverse.concepts("his colleague"));
    for (const UniqueId& colleague : colleagues)
    {
        otherUniverse.access(colleague).property("name", "Tobi");
        otherUniverse.access(colleague).property("age", "42");
    }
    REQUIRE(myDB.commit("myUniverse", otherUniverse) == true);

    // TODO: Open a new DB instance, open the same graph and check for equality?
}
