#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Conceptgraph.hpp"
#include "HypergraphYAML.hpp"

TEST_CASE("Construct a conceptual graph", "[Conceptgraph]")
{
    Conceptgraph cg;
    REQUIRE(cg.exists(Conceptgraph::IsRelationId) == true);
    REQUIRE(cg.isPointingFrom(Hyperedges{Conceptgraph::IsRelationId}) == Hyperedges{Conceptgraph::IsConceptId});
    REQUIRE(cg.concept("1", "My first concept") == Hyperedges{"1"});
    REQUIRE(cg.concept("2", "My second concept") == Hyperedges{"2"});
    REQUIRE(cg.concepts() == Hyperedges{"1", "2"});
    REQUIRE(cg.relate("R", Hyperedges{"1"}, Hyperedges{"2"}, "relatedTo") == Hyperedges{"R"});
    REQUIRE(intersect(cg.relationsFrom(Hyperedges{"1"}), Hyperedges{"R"}) == Hyperedges{"R"});
    REQUIRE(cg.isPointingTo(cg.relationsFrom(Hyperedges{"1"})) == Hyperedges{"2"});
    REQUIRE(cg.traverse(
        "1",
        [](const Conceptgraph& cg, const UniqueId& c, const Hyperedges& p) -> bool { return true; },
        [](const Conceptgraph& cg, const UniqueId& c, const UniqueId& r) -> bool {
            if (cg.access(r).label() == "relatedTo")
                return true;
            return false;
        }
    ) == Hyperedges{"1", "2"});
}
