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
    set.pointTo(&node);

    std::cout << "> Test 1-Hyperedge " << std::endl;
    auto members = set.pointingTo();
    assert(members.size() == 1);
    assert(members.begin()->second->label() == "A 0-hyperedge (empty named set)");

    std::cout << "*** Hyperedge Test Finished ***" << std::endl;

    std::cout << "> Creating (software) component modelling domain" << std::endl;

    // Create concepts
    Set things("Things");
    Set software("Software");
    Set components("Components");
    Set ports("Ports");
    Set inputs("Inputs");
    Set outputs("Outputs");

    // Build taxonomy using set system with isA relation (subsumption hierarchy)
    components.isA(&things);
    inputs.isA(&ports);
    outputs.isA(&ports);
    ports.isA(&things);

    // Compose something from simpler things with partOf relation (compositional hierarchy)
    // TODO: Ok, now ComponentX is a Component. What about Component1 and 2? Are they 'automatically' also components?
    auto composite = Set::create("ComponentX");
    Set::create("Component1")->partOf(composite);
    Set::create("Component2")->partOf(composite);
    composite->isA(&components);

    // Create own relation
    Relation has("have");
    has.from(&components);
    has.to(&ports);

    std::cout << "*** Id Test ***" << std::endl;

    Set* manufactured = things.labelContains<Set>();
    for (auto edgeIt : manufactured->pointingTo())
    {
        std::cout << edgeIt.second << std::endl;
        assert(edgeIt.first < manufactured->id());
    }
    delete manufactured;

    std::cout << "*** Id Test Finished ***" << std::endl;

    std::cout << "*** Built-in independent de-/serializer test ***" << std::endl;
    std::cout << "> Print things (using serializer)" << std::endl;
    std::cout << Hyperedge::serialize(&things) << std::endl;
    //std::cout << "> Print things (using serializer-deserializer-serializer compositional chain)" << std::endl;
    //std::cout << Hyperedge::serialize(Hyperedge::deserialize(Hyperedge::serialize(&things))) << std::endl;
    std::cout << "*** Built-in independent de-/serializer test finished ***" << std::endl;

    std::cout << "*** Queries Test ***" << std::endl;
    auto individuals = things.cardinalityLessThanOrEqual<Set>();
    std::cout << individuals << std::endl;
    auto special = things.cardinalityGreaterThan<Set>();
    std::cout << special << std::endl;

    std::cout << "*** Queries Test finished ***" << std::endl;

    std::cout << "> Cleanup" << std::endl;
    Hyperedge::cleanup();
    std::cout << "*** DONE ***" << std::endl;

    return 0;
}
