#include "Hyperedge.hpp"
#include "Set.hpp"
#include "Relation.hpp"

#include <iostream>
#include <cassert>

int main(void)
{
    std::cout << "*** Hyperedge Test ***" << std::endl;

    Hyperedge node("A 0-hyperedge (empty named set)");
    Hyperedge::Hyperedges members;
    members.push_back(&node);
    Hyperedge set(members, "A 1-hyperedge (set with one member)");

    std::cout << "> Test 1-Hyperedge " << std::endl;
    members.clear();
    members = set.members();
    assert(members.size() == 1);
    assert(members[0]->label() == "A 0-hyperedge (empty named set)");

    std::cout << "*** Hyperedge Test Finished ***" << std::endl;

    std::cout << "*** Derived Classes Test ***" << std::endl;

    std::cout << "> Creating persons" << std::endl;
    members.clear();
    members.push_back(new Set("John"));
    members.push_back(new Set("Bob"));
    members.push_back(new Set("Alice"));
    std::cout << "> Creating person set" << std::endl;
    Set persons(members, "Persons");
    Relation related(members, "related");
    std::cout << "> Creating set of sets" << std::endl;
    members.clear();
    members.push_back(&persons);
    Set things(members, "Things");
    std::cout << "> Searching for persons" << std::endl;
    members.clear();
    members = persons.members("Alice");
    members.push_back(persons.members("Bob")[0]); 
    std::cout << "> Creating relation between persons" << std::endl;
    Relation married(members, "married"); // NOTE: This states: "Every Alice is married to one Bob"
    members.clear();
    members.push_back(&related);
    Set rel1(members, "Relatives");
    members.clear();
    members.push_back(&married);
    Set rel2(members, "Couples");
    members.clear();
    members.push_back(&rel1);
    members.push_back(&rel2);
    auto oldMembers = persons.members();
    members.insert(members.end(), oldMembers.begin(), oldMembers.end());
    persons = Set(members, persons.label()); // This is an update
    std::cout << "*** Derived Classes Test Finished ***" << std::endl;

    std::cout << "*** Built-in independent de-/serializer test ***" << std::endl;
    std::cout << "> Print things (using serializer)" << std::endl;
    std::cout << Hyperedge::serialize(&things) << std::endl;
    std::cout << "> Print things (using serializer-deserializer-serializer compositional chain)" << std::endl;
    std::cout << Hyperedge::serialize(Hyperedge::deserialize(Hyperedge::serialize(&things))) << std::endl;
    std::cout << "*** Built-in independent de-/serializer test finished ***" << std::endl;

    std::cout << "*** Queries Test ***" << std::endl;
    auto individuals = things.query();
    std::cout << Hyperedge::serialize(&individuals) << std::endl;
    auto special = things.query("o",0,"Special");
    std::cout << Hyperedge::serialize(&special) << std::endl;
    std::cout << "*** Queries Test finished***" << std::endl;

    return 0;
}
