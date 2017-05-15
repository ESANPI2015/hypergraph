#include "Hyperedge.hpp"
#include "Set.hpp"
#include "Relation.hpp"
#include "HyperedgeYAML.hpp"

#include <fstream>
#include <iostream>
#include <cassert>

int main(void)
{
    std::cout << "*** Hyperedge Test ***" << std::endl;

    Hyperedge* node = Hyperedge::create("A 0-hyperedge (empty named set)");
    Hyperedge* set = Hyperedge::create("A 1-hyperedge (set with one member)");
    set->pointTo(node);

    std::cout << "> Test 1-Hyperedge " << std::endl;
    auto members = set->pointingTo();
    assert(members.size() == 1);

    std::cout << "*** Hyperedge Test Finished ***" << std::endl;

    std::cout << "> Creating (software) component modelling domain" << std::endl;

    // Create concepts
    Set* things = Set::create("Things");
    Set* software = Set::create("Software");
    Set* components = Set::create("Components");
    Set* ports = Set::create("Ports");
    Set* inputs = Set::create("Inputs");
    Set* outputs = Set::create("Outputs");

    // Build taxonomy using set system with isA relation (subsumption hierarchy)
    components->isA(things);
    inputs->isA(ports);
    outputs->isA(ports);
    ports->isA(things);

    components->memberOf(software);

    // Compose something from simpler things with partOf relation (compositional hierarchy)
    // TODO: Ok, now ComponentX is a Component. What about Component1 and 2? Are they 'automatically' also components?
    auto composite = Set::create("ComponentX");
    Set::create("Component1")->partOf(composite);
    Set::create("Component2")->partOf(composite);
    composite->isA(components);

    // Create own relation
    Relation* has = Relation::create("have");
    has->from(components);
    has->to(ports);

    std::cout << "*** Id Test ***" << std::endl;

    auto manufactured = things->labelContains();
    for (auto edgeId : manufactured->pointingTo())
    {
        std::cout << Hyperedge::find(edgeId) << std::endl;
        assert(edgeId < manufactured->id());
    }
    //delete manufactured;

    std::cout << "*** Id Test Finished ***" << std::endl;

    std::cout << "*** Built-in independent de-/serializer test ***" << std::endl;
    std::cout << "> Print things (using serializer)" << std::endl;
    std::cout << Hyperedge::serialize(things) << std::endl;
    //std::cout << "> Print things (using serializer-deserializer-serializer compositional chain)" << std::endl;
    //std::cout << Hyperedge::serialize(Hyperedge::deserialize(Hyperedge::serialize(&things))) << std::endl;
    std::cout << "*** Built-in independent de-/serializer test finished ***" << std::endl;

    std::cout << "*** Queries Test ***" << std::endl;
    auto individuals = things->cardinalityLessThanOrEqual();
    std::cout << individuals << std::endl;
    auto special = things->cardinalityGreaterThan();
    std::cout << special << std::endl;

    std::cout << "*** Queries Test finished ***" << std::endl;

    std::cout << "*** YAML Test ***" << std::endl;

    std::cout << "> From Hyperedge(s) to YAML\n";
    YAML::Node test;
    test = static_cast<Hyperedge*>(composite);
    std::cout << test << std::endl;
    
    std::cout << "> From YAML to Hyperedge(s)\n";
    Hyperedge *wurst = test.as<Hyperedge*>();
    std::cout << Hyperedge::serialize(wurst) << std::endl;

    std::cout << "> Store everything to YAML file\n";
    std::ofstream fout;
    fout.open("test.yml");
    if(fout.good()) {
        fout << YAML::store(things);
    } else {
        std::cout << "FAILED\n";
    }
    fout.close();
    
    std::cout << "> Cleanup" << std::endl;
    Hyperedge::cleanup();
    // NOTE: After here nothing exists anymore!

    test.reset();
    std::cout << "> Load from YAML file\n";
    test = YAML::LoadFile("test.yml");
    auto edges = YAML::load(test);
    wurst = Hyperedge::find(*(Hyperedge::find(*edges.begin())->labelPartOf("ComponentX")->pointingTo()).begin());
    assert(wurst != NULL);
    std::cout << Hyperedge::serialize(wurst) << std::endl;

    std::cout << "*** YAML Test finished ***" << std::endl;

    std::cout << "> Cleanup" << std::endl;
    Hyperedge::cleanup();
    std::cout << "*** DONE ***" << std::endl;

    return 0;
}
