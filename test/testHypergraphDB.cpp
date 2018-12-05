#include "CommonConceptGraph.hpp"
#include "HypergraphDB.hpp"
#include "HypergraphYAML.hpp"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <fstream>
#include <iostream>
#include <cassert>

int main(void)
{
    std::cout << "*** HYPERGRAPH DB TEST ***" << std::endl;

    std::cout << "> Create DB\n";
    HypergraphDB myDB;

    std::cout << "> Create common concept graph\n";
    CommonConceptGraph universe;
    universe.create("PERSON", "Person");
    universe.instantiateFrom("PERSON", "Moritz Schilling");
    universe.instantiateFrom("PERSON", "and his colleague");

    std::cout << "> Commit to DB\n";
    myDB.commit("myUniverse", universe);

    std::cout << "> Open from DB\n";
    CommonConceptGraph otherUniverse(myDB.load("myUniverse"));

    std::cout << "> Modify graph\n";
    Hyperedges colleagues(otherUniverse.find("and his colleague"));
    for (UniqueId colleague : colleagues)
    {
        otherUniverse.get(colleague).updateLabel("Tobi");
    }

    std::cout << "> Recommit to DB\n";
    myDB.commit("myUniverse", otherUniverse);
    
    std::cout << "*** TESTS DONE ***" << std::endl;

    return 0;
}
