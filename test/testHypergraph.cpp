#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Hyperedge.hpp"
#include "Hypergraph.hpp"
#include "HypergraphYAML.hpp"

#include <iostream>

TEST_CASE("Construct an hypergraph", "[Hypergraph]")
{
    Hypergraph hg;
    REQUIRE(hg.exists(Hypergraph::Zero) == true);
    REQUIRE(hg.size() == 1);
    const Hyperedge invalid(hg.access("Non-existent"));
    REQUIRE(invalid.id() == Hypergraph::Zero); 
    REQUIRE(hg.create("1", "My first hedge").empty() == false);
    REQUIRE(hg.access("1").id() == "1");
    REQUIRE(hg.access("1").label() == "My first hedge");
    REQUIRE(hg.create("2", "My second hedge").empty() == false);
    REQUIRE(hg.findByLabel().size() == 3);
    REQUIRE(hg.findByLabel("My first hedge").size() == 1);
    REQUIRE(hg.pointsTo(Hyperedges{"1"}, Hyperedges{"2"}).size() == 2);
    REQUIRE(hg.isPointingTo(Hyperedges{"1"}).size() == 1);
    REQUIRE(hg.traverse(
        "1",
        [](const Hypergraph& hg, const UniqueId& x, const Hyperedges& p) -> bool { return true; },
        [](const Hypergraph& hg, const UniqueId& x, const UniqueId& y) -> bool { return true; }
    ) == Hyperedges{"1", "2"});
    // TODO: Test pattern matching
    SECTION("Pattern matching")
    {
    }
    // TODO: Test rewriting
    SECTION("Rewriting")
    {
    }
    SECTION("Serialize & Reconstruct")
    {
        const std::string& serialized(YAML::StringFrom(hg));
        Hypergraph reconstructed(YAML::Load(serialized).as<Hypergraph>());
        for (const UniqueId& a : hg.findByLabel())
        {
            REQUIRE(reconstructed.exists(a) == true);
            REQUIRE(hg.isPointingFrom(Hyperedges{a}) == reconstructed.isPointingFrom(Hyperedges{a}));
            REQUIRE(hg.isPointingTo(Hyperedges{a}) == reconstructed.isPointingTo(Hyperedges{a}));
        }
    }
}
