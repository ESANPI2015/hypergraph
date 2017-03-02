#include "Hyperedge.hpp"
#include "Set.hpp"
#include "Individual.hpp"
#include "Fact.hpp"

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

    std::cout << "> Creating individuals" << std::endl;
    members.clear();
    members.push_back(new Individual("John"));
    members.push_back(new Individual("Bob"));
    members.push_back(new Individual("Alice"));
    std::cout << "> Creating set of individuals" << std::endl;
    Set persons(members, "Persons");
    Fact related(members, "related"); // NOTE: Here it looks like Set and Fact are the same, but they are not!
    std::cout << "> Creating set of sets" << std::endl;
    members.clear();
    members.push_back(&persons);
    Set things(members, "Things");
    std::cout << "> Searching for individuals" << std::endl;
    members.clear();
    members = persons.members("Alice");
    members.push_back(persons.members("Bob")[0]); 
    std::cout << "> Creating fact between individuals" << std::endl;
    Fact married(members, "married"); // NOTE: This states: "Every Alice is married to one Bob"
    members.clear();
    members.push_back(&related);
    Set rel1(members, "Related");
    members.clear();
    members.push_back(&married);
    Set rel2(members, "Married");
    members.clear();
    members.push_back(&rel1);
    members.push_back(&rel2);
    Set relations(members, "Relations");
    members.clear();
    members.push_back(&persons);
    members.push_back(&relations);
    things = Set(members, "Things");

    members.clear();
    members = things.members();
    std::cout << "> Print things" << std::endl;
    //while (members.size())
    //{
    //    // pop top of stack
    //    auto member = members.front();
    //    members.erase(members.begin());
    //    auto others = member->members();
    //    std::cout << "\t" << member->label() << ": " << others.size() << std::endl;
    //    members.insert(members.begin(), others.begin(), others.end());
    //}
    std::cout << things.serialize() << std::endl;

    std::cout << "*** Derived Classes Test Finished ***" << std::endl;

    return 0;
}
