#include "HypergraphDB.hpp"

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
    
    // TODO: Check if git folder exists

    std::cout << "*** TESTS DONE ***" << std::endl;

    return 0;
}
