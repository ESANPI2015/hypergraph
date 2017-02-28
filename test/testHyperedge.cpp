#include "Hyperedge.hpp"

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
    return 0;
}
