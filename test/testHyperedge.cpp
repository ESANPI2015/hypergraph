#include "Hyperedge.hpp"
#include "Set.hpp"
#include "Relation.hpp"

#include <iostream>
#include <cassert>

int main(void)
{
    std::cout << "*** Hyperedge Test ***" << std::endl;

    Hyperedge node("A 0-hyperedge (empty named set)");
    Hyperedge set("A 1-hyperedge (set with one member)");
    set.contains(&node);

    std::cout << "> Test 1-Hyperedge " << std::endl;
    auto members = set.members();
    assert(members.size() == 1);
    assert(members.begin()->second->label() == "A 0-hyperedge (empty named set)");

    std::cout << "*** Hyperedge Test Finished ***" << std::endl;

    std::cout << "> Creating software component modelling domain" << std::endl;

    // Create concepts
    Set things("Things");
    Set software("Software");
    Set components("Components");
    Set ports("Ports");
    Set inputs("Inputs");
    Set outputs("Outputs");

    // Build taxonomy
    things.contains(&components);
    ports.contains(&inputs);
    ports.contains(&outputs);
    components.contains(&software);
    things.contains(&ports);

    // Create relations
    Relation has("have");
    has.contains(&components);
    has.contains(&ports);

    std::cout << "*** Id Test ***" << std::endl;

    auto manufactured = Hyperedge::create("Dynamic");
    for (auto edgeIt : things.labelContains())
    {
        std::cout << "ID: " << edgeIt.first << " LABEL: " << edgeIt.second->label() << std::endl;
        assert(edgeIt.first < manufactured->id());
    }

    std::cout << "*** Id Test Finished ***" << std::endl;

    std::cout << "*** Built-in independent de-/serializer test ***" << std::endl;
    std::cout << "> Print things (using serializer)" << std::endl;
    std::cout << Hyperedge::serialize(&things) << std::endl;
    std::cout << "> Print things (using serializer-deserializer-serializer compositional chain)" << std::endl;
    std::cout << Hyperedge::serialize(Hyperedge::deserialize(Hyperedge::serialize(&things))) << std::endl;
    std::cout << "*** Built-in independent de-/serializer test finished ***" << std::endl;

    std::cout << "*** Queries Test ***" << std::endl;
    auto individuals = Hyperedge::create(things.cardinalityLessThanOrEqual(), "Individuals");
    std::cout << Hyperedge::serialize(individuals) << std::endl;
    auto special = Hyperedge::create(things.cardinalityGreaterThan(), "Others");;
    std::cout << Hyperedge::serialize(special) << std::endl;

    std::cout << "*** Queries Test finished ***" << std::endl;

    std::cout << "> Cleanup" << std::endl;
    Hyperedge::cleanup();
    std::cout << "*** DONE ***" << std::endl;

    return 0;
}
